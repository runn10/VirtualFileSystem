#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#include <string>
#include "IOSystem.h"
#include "DescriptorTable.h"
#include "OpenFile.h"
#include "DirectoryEntry.h"
#include "DiskStatus.h"

class FileSystem
{
public:
	FileSystem(IOSystem& io, std::string storage);
	~FileSystem();

	inline DiskStatus getDiskStatus() { return this->disk_status; }

	// Create a new file with the specified filename
	int create(std::string symbolic_file_name);

	// Destroy the named file.
	int destroy(std::string symbolic_file_name);

	// Open the named file for reading and writing; 
	// Return an index value which is used by subsequent read, write, lseek, or close operations.
	int open(std::string symbolic_file_name);

	// Close the specified file.
	int close(int index);

	// sequentially read a number of bytes from the specified
	// file into main memory.The number of bytes to be read is specified in count
	// and the starting memory address in mem area.The reading starts with the current
	// position in the file.
	int read(int index, char* mem_area, int count);

	// sequentially write a number of bytes from main
	// memory starting at mem area into the specified file.As with the read operation,
	// the number of bytes is given in count and the writing begins with the current
	// position in the file.
	int write(int index, char value, int count);

	// move the current position of the file to pos, where pos is an
	// integer specifying the number of bytes from the beginning of the file.When a file
	// is initially opened, the current position is automatically set to zero.After each read
	// or write operation, it points to the byte immediately following the one that was
	// accessed last.lseek permits the position to be explicitly changed without reading
	// or writing the data.Seeking to position 0 implements a reset command, so that the
	// entire file can be reread or rewritten from the beginning.
	int lseek(int index, int pos);

	// lists the names of all files and their lengths.
	void directory();

	// saves state of FileSystem to a file
	void save(std::string filename);

	///////////////////////////////////////////////////////
	// TEST FUNCTIONS /////////////////////////////////////
	///////////////////////////////////////////////////////

	DirectoryEntry* getDirectoryEntries(int& size);
	DescriptorTable* getDescriptorTable();
	void diskdump(int start, int end);

private:
	IOSystem& io;
	DescriptorTable desc_table;
	static const int OFT_Size = 4;
	OpenFile* OFT[OFT_Size];
	DiskStatus disk_status;

	static const int directoryIndex = 0;
	static const int maxFileSize = 192;
	
	///////////////////////////////////////////////////////
	// HELPER FUNCTIONS ///////////////////////////////////
	///////////////////////////////////////////////////////

	int findFreeOFTindex();
	DirectoryEntry getDirectoryEntry(int index);
	DirectoryEntry findFileInDirectory(std::string symbolic_file_name);
	DirectoryEntry findFreeDirectoryEntry();
	bool fileExists(std::string symbolic_file_name);
	void writeDirectoryEntry(DirectoryEntry entry);
	void removeDirectoryEntry(DirectoryEntry entry);
	std::string padName(std::string name);
};

#endif//FILESYSTEM_HPP