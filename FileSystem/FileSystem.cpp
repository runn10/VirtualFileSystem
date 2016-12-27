#include <algorithm> //for std::min()
#include <iostream>
#include <string>
#include <fstream>

#include "FileSystem.h"
#include "Errors.h"
#include "DirectoryEntry.h"


FileSystem::FileSystem(IOSystem& io, std::string storage) : io(io), desc_table(io)
{

	std::ifstream file(storage);
	bool fileExists = file.good();
	file.close();

	if (fileExists)
	{
		io.load(storage);
		desc_table = DescriptorTable(io);

		//Open The Directory File.
		FileDescriptor DirectoryDescriptor = desc_table.getFileDescriptor(0);
		OFT[0] = new OpenFile(DirectoryDescriptor.getDescriptorIndex());
		io.read_block(DirectoryDescriptor.getFirstBlock(), OFT[0]->RW_Buffer);

		disk_status = DiskStatus::RESTORED;
	}
	else
	{
		//Initialize the directory. The directory is an always-open file.
		FileDescriptor DirectoryDescriptor = desc_table.getFileDescriptor(0);
		DirectoryDescriptor.setFirstBlock(desc_table.getBitmap().findFreeBlock());
		desc_table.getBitmap().setBlock(DirectoryDescriptor.getFirstBlock());
		OFT[0] = new OpenFile(DirectoryDescriptor.getDescriptorIndex());
		desc_table.UpdateLdisk();
		disk_status = DiskStatus::INITIALIZED;
	}
}

FileSystem::~FileSystem()
{
	for (int i = 0; i < 4; i++)
		close(i);
}

int FileSystem::create(std::string symbolic_file_name)
{
	if ((symbolic_file_name = padName(symbolic_file_name)).length() > 4) 
		return ERROR.NAME_TOO_LONG;
	if (fileExists(symbolic_file_name))
		return ERROR.DUPLICATE_FILE;
	try
	{
		//Get a new file descriptor and find an empty block for the new file.
		FileDescriptor fileDescriptor = desc_table.findFreeFileDescriptor(); //find free file descriptor
		fileDescriptor.setFirstBlock(desc_table.getBitmap().findFreeBlock()); // find free block
		desc_table.getBitmap().setBlock(fileDescriptor.getFirstBlock()); // update bitmap

		//find a free entry in the directory and write the new file info.
		DirectoryEntry entry = findFreeDirectoryEntry();
		entry.setName(symbolic_file_name);
		entry.setDescriptorIndex(fileDescriptor.getDescriptorIndex());
		writeDirectoryEntry(entry);
		desc_table.UpdateLdisk();
	}
	catch (char exception)
	{
		if (exception == EXCEPTION.NO_FREE_FILE_DESCRIPTORS) return ERROR.NO_SPACE_ON_DISK;
		if (exception == EXCEPTION.NO_ROOM_IN_DIRECTORY) return ERROR.NO_SPACE_ON_DISK;
	}
	return 0;
}

int FileSystem::destroy(std::string symbolic_file_name)
{
	if ((symbolic_file_name = padName(symbolic_file_name)).length() > 4)
		return ERROR.NAME_TOO_LONG;

	try
	{
		DirectoryEntry directoryEntry = findFileInDirectory(symbolic_file_name);
		FileDescriptor fileDescriptor = desc_table.getFileDescriptor(directoryEntry.getDescriptorIndex());

		//update bitmap
		if (fileDescriptor.getFirstBlock() > 0) desc_table.getBitmap().freeBlock(fileDescriptor.getFirstBlock());
		if (fileDescriptor.getSecondBlock() > 0) desc_table.getBitmap().freeBlock(fileDescriptor.getSecondBlock());
		if (fileDescriptor.getThirdBlock() > 0) desc_table.getBitmap().freeBlock(fileDescriptor.getThirdBlock());

		removeDirectoryEntry(directoryEntry);
		desc_table.freeFileDescriptor(fileDescriptor);
		desc_table.UpdateLdisk();
	}
	catch (char exception)
	{
		if (exception == EXCEPTION.FILE_NOT_FOUND) return ERROR.FILE_NOT_FOUND;
	}
	return 0;
}

int FileSystem::open(std::string symbolic_file_name)
{
	if ((symbolic_file_name = padName(symbolic_file_name)).length() > 4)
		return ERROR.NAME_TOO_LONG;

	try
	{
		DirectoryEntry directoryEntry = findFileInDirectory(symbolic_file_name);
		FileDescriptor fileDescriptor = desc_table.getFileDescriptor(directoryEntry.getDescriptorIndex());

		if (isFileOpen(directoryEntry))
			return ERROR.FILE_ALREADY_OPEN;

		int OFT_Index = findFreeOFTindex();
		OFT[OFT_Index] = new OpenFile(fileDescriptor.getDescriptorIndex());
		io.read_block(fileDescriptor.getFirstBlock(), OFT[OFT_Index]->RW_Buffer);
		return OFT_Index;
	}
	catch (char exception) 
	{
		if (exception == EXCEPTION.FILE_NOT_FOUND) return ERROR.FILE_NOT_FOUND;
		if (exception == EXCEPTION.NO_FREE_OFT_ENTRY) return ERROR.NO_FREE_OFT_ENTRY;
	}
}

int FileSystem::close(int index)
{
	if (index < 0 || index >= OFT_Size) return ERROR.FILE_IS_NOT_OPEN;
	if (OFT[index] == nullptr) return ERROR.FILE_IS_NOT_OPEN;

	OpenFile* openFile = OFT[index];
	FileDescriptor fd = desc_table.getFileDescriptor(openFile->fileDescriptorIndex);
	int blockIndex = openFile->currentPosition / openFile->BufferSize;
	int fileBlock = blockIndex >= 3 ? fd.getBlock(2) : fd.getBlock(blockIndex);
	io.write_block(fileBlock, openFile->RW_Buffer);
	delete openFile;
	OFT[index] = nullptr;
	return 0;
}

int FileSystem::read(int index, char* mem_area, int count)
{
	if (index < 0 || index >= OFT_Size) return ERROR.FILE_IS_NOT_OPEN;
	if (OFT[index] == nullptr) return ERROR.FILE_IS_NOT_OPEN;

	OpenFile* openFile = OFT[index];
	FileDescriptor fd = desc_table.getFileDescriptor(openFile->fileDescriptorIndex);
	int bytesRead = 0;
	
	while (count > 0)
	{
		//EOF?
		if (openFile->currentPosition >= fd.getFileSize())
			return ERROR.END_OF_FILE;

		//some calculations
		int buffer_pos = openFile->currentPosition % openFile->BufferSize;
		int blockIndex = openFile->currentPosition / openFile->BufferSize;
		int fileBlock = fd.getBlock(blockIndex);
		int toRead = std::min(count, openFile->BufferSize - buffer_pos); //end of buffer.
		toRead = std::min(toRead, fd.getFileSize() - openFile->currentPosition); //end of file.
		bool finishedCurrentBlock = (toRead == openFile->BufferSize - buffer_pos);
		
		//read available bytes.
		std::memcpy(mem_area, openFile->RW_Buffer + buffer_pos, toRead);
		mem_area += toRead;
		openFile->currentPosition += toRead;
		bytesRead += toRead;
		count -= toRead;

		// reached end of block?
		if (finishedCurrentBlock) io.write_block(fileBlock, openFile->RW_Buffer);
		int newBlockIndex = openFile->currentPosition / openFile->BufferSize;
		if (finishedCurrentBlock && newBlockIndex < 3)
		{
			int nextFileBlock = fd.getBlock(newBlockIndex);
			if (nextFileBlock > 0) io.read_block(nextFileBlock, openFile->RW_Buffer);
			else throw EXCEPTION.CONTROL_FLOW_BUG;
		}
	}

	return bytesRead;
}

int FileSystem::write(int index, char value, int count)
{
	if (index < 0 || index >= OFT_Size) return ERROR.FILE_IS_NOT_OPEN;
	if (OFT[index] == nullptr) return ERROR.FILE_IS_NOT_OPEN;

	OpenFile* openFile = OFT[index];
	FileDescriptor fd = desc_table.getFileDescriptor(openFile->fileDescriptorIndex);
	int bytesWritten = 0;

	while (count > 0)
	{
		//Max file size?
		if (openFile->currentPosition == maxFileSize)
		{
			fd.setFileSize(maxFileSize);
			desc_table.UpdateLdisk();
			return ERROR.MAX_FILESIZE_REACHED;
		}

		//some calculations
		int buffer_pos = openFile->currentPosition % openFile->BufferSize;
		int blockIndex = openFile->currentPosition / openFile->BufferSize;
		int fileBlock = fd.getBlock(blockIndex);
		int toWrite = std::min(count, openFile->BufferSize - buffer_pos);
		bool filledCurrentBlock = (toWrite == openFile->BufferSize - buffer_pos);

		// write bytes.
		for (int i = 0; i < toWrite; i++)		openFile->RW_Buffer[buffer_pos + i] = value;
		openFile->currentPosition += toWrite;
		bytesWritten += toWrite;
		count -= toWrite;

		// reached end of block?
		if (filledCurrentBlock) io.write_block(fileBlock, openFile->RW_Buffer);
		int newBlockIndex = openFile->currentPosition / openFile->BufferSize;
		if (filledCurrentBlock && newBlockIndex < 3)
		{
			int nextFileBlock = fd.getBlock(newBlockIndex);
			if (nextFileBlock == 0) // Need to allocate new block?
			{
				int nextFileBlock = desc_table.getBitmap().findFreeBlock();
				desc_table.getBitmap().setBlock(nextFileBlock);
				fd.setBlock(newBlockIndex, nextFileBlock);
			}
			io.read_block(nextFileBlock, openFile->RW_Buffer);
		}
	}

	fd.setFileSize(std::max(fd.getFileSize(), openFile->currentPosition));
	desc_table.UpdateLdisk();
	return bytesWritten;
}

int FileSystem::lseek(int index, int pos)
{
	if (index < 0 || index >= OFT_Size) return ERROR.FILE_IS_NOT_OPEN;
	if (OFT[index] == nullptr) return ERROR.FILE_IS_NOT_OPEN;

	OpenFile* openFile = OFT[index];
	FileDescriptor fd = desc_table.getFileDescriptor(openFile->fileDescriptorIndex);
	if (pos < 0 || pos > fd.getFileSize()) return ERROR.POSITION_OUT_OF_BOUNDS;

	int currentBlock = (openFile->currentPosition == maxFileSize) ? 
		fd.getBlock(2) : fd.getBlock(openFile->currentPosition / openFile->BufferSize);
	int destinationBlock = (pos == maxFileSize) ? 
		fd.getBlock(2) : fd.getBlock(pos / openFile->BufferSize);
	if (currentBlock != destinationBlock)
	{
		io.write_block(currentBlock, openFile->RW_Buffer);
		io.read_block(destinationBlock, openFile->RW_Buffer);
	}
	openFile->currentPosition = pos;

	return 0;
}

void FileSystem::directory(std::ofstream& out)
{
	int count = 0;
	DirectoryEntry* entries = getDirectoryEntries(count);
	for (int i = 0; i < count; i++)
	{
		DirectoryEntry de = entries[i];
		out << de.getName() << '\t';
	}
	out << "\n";
	delete[] entries;
}

void FileSystem::save(std::string filename)
{
	// flush any changes in OpenFile buffers.
	for (int i = 0; i < 4; i++)
	{
		OpenFile* openFile = OFT[i];
		if (openFile == nullptr) continue;
		FileDescriptor fd = desc_table.getFileDescriptor(openFile->fileDescriptorIndex);
		int blockIndex = openFile->currentPosition / openFile->BufferSize;
		int fileBlock = blockIndex >= 3 ? fd.getBlock(2) : fd.getBlock(blockIndex);
		io.write_block(fileBlock, openFile->RW_Buffer);
	}
	
	//write descriptor table to Ldisk
	desc_table.UpdateLdisk();

	//save Ldisk to an actual file.
	io.save(filename);
}

///////////////////////////////////////////////////////
// TESTING FUNCTIONS //////////////////////////////////
///////////////////////////////////////////////////////

DirectoryEntry* FileSystem::getDirectoryEntries(int& size)
{
	FileDescriptor DirectoryDescriptor = desc_table.getFileDescriptor(0);
	int filesInDirectory = DirectoryDescriptor.getFileSize() / 8;
	DirectoryEntry* entries = new DirectoryEntry[filesInDirectory];
	size = 0;
	for (int i = 0; i < filesInDirectory; i++)
	{
		DirectoryEntry de = getDirectoryEntry(i);
		if (de.getDescriptorIndex() > 0)
			entries[size++] = de;
	}
	return entries;
}

DescriptorTable* FileSystem::getDescriptorTable()
{
	return &desc_table;
}

void FileSystem::diskdump(int start, int end)
{
	
	io.diskdump(start, end);
}


///////////////////////////////////////////////////////
// HELPER FUNCTIONS ///////////////////////////////////
///////////////////////////////////////////////////////

bool FileSystem::fileExists(std::string symbolic_file_name)
{
	try
	{
		findFileInDirectory(symbolic_file_name);
		return true;
	}
	catch (char exception)
	{
		if (exception == EXCEPTION.FILE_NOT_FOUND) 
			return false;
	}
	return false;
}

DirectoryEntry FileSystem::getDirectoryEntry(int index)
{
	FileDescriptor DirectoryDescriptor = desc_table.getFileDescriptor(0);
	int filesInDirectory = DirectoryDescriptor.getFileSize() / 8;
	if (index >= filesInDirectory) throw EXCEPTION.INDEX_OUT_OF_RANGE;

	int position = index * 8;
	lseek(directoryIndex, position);
	char entry[8];
	read(directoryIndex, entry, 8);
	return DirectoryEntry(position, entry);
}

DirectoryEntry FileSystem::findFileInDirectory(std::string symbolic_file_name)
{
	if (symbolic_file_name.length() > 4) throw EXCEPTION.NAME_TOO_LONG;
	FileDescriptor DirectoryDescriptor = desc_table.getFileDescriptor(0);
	int filesInDirectory = DirectoryDescriptor.getFileSize() / 8;
	lseek(directoryIndex, 0);
	for (int i = 0; i < filesInDirectory; i++)
	{
		DirectoryEntry entry = getDirectoryEntry(i);
		if (entry.getName().compare(symbolic_file_name) == 0 && entry.getDescriptorIndex() > 0)
			return entry;
	}
	throw EXCEPTION.FILE_NOT_FOUND;
}

DirectoryEntry FileSystem::findFreeDirectoryEntry()
{
	FileDescriptor DirectoryDescriptor = desc_table.getFileDescriptor(0);
	int filesInDirectory = DirectoryDescriptor.getFileSize() / 8;
	lseek(directoryIndex, 0);
	for (int i = 0; i < filesInDirectory; i++)
	{
		DirectoryEntry entry = getDirectoryEntry(i);
		if (entry.getDescriptorIndex() == 0)
			return entry;
	}
	if (DirectoryDescriptor.getFileSize() + 8 > maxFileSize)
		throw EXCEPTION.NO_ROOM_IN_DIRECTORY;
	
	char empty[8]{ 0, 0, 0, 0, 0, 0, 0, 0 };
	return DirectoryEntry(DirectoryDescriptor.getFileSize(), empty);
}

void FileSystem::writeDirectoryEntry(DirectoryEntry entry)
{
	std::string stringName = entry.getName();
	const char* name = stringName.c_str();
	lseek(directoryIndex, entry.getPosition());
	write(directoryIndex, name[0], 1);
	write(directoryIndex, name[1], 1);
	write(directoryIndex, name[2], 1);
	write(directoryIndex, name[3], 1);

	int descriptorIndex = entry.getDescriptorIndex();
	const char* index = (const char*)(&descriptorIndex);
	write(directoryIndex, index[0], 1);
	write(directoryIndex, index[1], 1);
	write(directoryIndex, index[2], 1);
	write(directoryIndex, index[3], 1);
}

void FileSystem::removeDirectoryEntry(DirectoryEntry entry)
{
	entry.setDescriptorIndex(0);
	writeDirectoryEntry(entry);
}

int FileSystem::findFreeOFTindex()
{
	for (int i = 0; i < 4; i++)
		if (OFT[i] == nullptr)
			return i;
	throw EXCEPTION.NO_FREE_OFT_ENTRY;
}

std::string FileSystem::padName(std::string name)
{
	return name.length() >= 4 ? name : name.insert(name.length(), 4 - name.length(), '\0');
}


bool FileSystem::isFileOpen(DirectoryEntry de)
{
	int idx = 0;
	bool flag = false;
	for (int i = 0; i < 4; ++i)
	{
		if (OFT[i] == NULL)
			break;
		else
		{
			idx = OFT[i]->fileDescriptorIndex;
			if (de.getDescriptorIndex() == idx)
			{
				flag = true;
				break;
			}
		}
	}

	return flag;
}