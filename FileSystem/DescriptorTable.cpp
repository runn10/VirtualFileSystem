#include "DescriptorTable.h"
#include "Errors.h"
#include <cmath>

DescriptorTable::DescriptorTable(IOSystem& io) : io(io)
{
	cache = new char*[K];
	for (int i = 0; i < K; i++)
	{
		cache[i] = new char[io.B];
		io.read_block(i, cache[i]);
	}

	//Set the first K blocks as occupied.
	Bitmap bitmap = getBitmap();
	for (int i = 0; i < K; i++)
		bitmap.setBlock(i);
}

DescriptorTable::DescriptorTable(const DescriptorTable& copy) : io(copy.io)
{
	cache = new char*[K];
	for (int i = 0; i < K; i++)
	{
		cache[i] = new char[io.B];
		io.read_block(i, cache[i]);
	}

	//Set the first K blocks as occupied.
	Bitmap bitmap = getBitmap();
	for (int i = 0; i < K; i++)
		bitmap.setBlock(i);
}

DescriptorTable& DescriptorTable::operator=(const DescriptorTable& copy)
{
	if (this != &copy)
	{
		for (int i = 0; i < K; i++)
		for (int j = 0; j < io.B; j++)
			this->cache[i][j] = copy.cache[i][j];
	}

	return *this;
}

DescriptorTable::~DescriptorTable()
{
	for (char** p = cache; p < cache + K; p++)
		delete[] *p;
	delete[] cache;
}

//Called at the end of each:
//FileSystem(),
//FileSystem.Create(),
//FileSystem.Destroy(),
//FileSystem.Write()

void DescriptorTable::UpdateLdisk()
{
	for (int i = 0; i < K; i++)
		io.write_block(i, cache[i]);
}

Bitmap DescriptorTable::getBitmap()
{
	return Bitmap(cache[0], K, io.L);
}

FileDescriptor DescriptorTable::findFreeFileDescriptor()
{
	for (int i = 0; i < DescriptorCount; i++)
	{
		FileDescriptor fd = getFileDescriptor(i);
		if (fd.isFree()) return fd;
	}
	throw EXCEPTION.NO_FREE_FILE_DESCRIPTORS;
}

FileDescriptor DescriptorTable::getFileDescriptor(int descriptorIndex)
{
	if (descriptorIndex >= DescriptorCount)
		throw "Descriptor Index out of bounds";
	int blockNumber = descriptorIndex / 4 + 1;
	int BlockLocation = 16 * (descriptorIndex % 4);
	return FileDescriptor(descriptorIndex, (int*)(cache[blockNumber] + BlockLocation));
}

void  DescriptorTable::freeFileDescriptor(FileDescriptor fileDescriptor)
{
	fileDescriptor.setFileSize(0);
	fileDescriptor.setFirstBlock(0);
	fileDescriptor.setSecondBlock(0);
	fileDescriptor.setThirdBlock(0);
}