#include "FileDescriptor.h"
#include "Errors.h"

FileDescriptor::FileDescriptor(int descriptorIndex, int* cache_location) :
	descriptorIndex(descriptorIndex),
	cache_location(cache_location) { }

bool FileDescriptor::isFree()
{
	return getFileSize() == 0
		&& getFirstBlock() == 0
		&& getSecondBlock() == 0
		&& getThirdBlock() == 0;
}

int FileDescriptor::getBlock(int i)
{
	switch (i)
	{
	case 0: return getFirstBlock();
		break;
	case 1: return getSecondBlock();
		break;
	case 2: return getThirdBlock();
		break;
	default:
		throw EXCEPTION.INDEX_OUT_OF_RANGE;
	}
}


int FileDescriptor::setBlock(int i, int BlockNumber)
{
	switch (i)
	{
	case 0: setFirstBlock(BlockNumber);
		break;
	case 1: setSecondBlock(BlockNumber);
		break;
	case 2: setThirdBlock(BlockNumber);
		break;
	default:
		throw EXCEPTION.INDEX_OUT_OF_RANGE;
	}
}