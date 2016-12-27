#ifndef FILEDESCRIPTOR_HPP
#define FILEDESCRIPTOR_HPP

class FileDescriptor
{
public:
	FileDescriptor(int descriptorIndex, int* cache_location);
	bool isFree();

	inline int getDescriptorIndex() { return descriptorIndex; }

	inline int getFileSize()	{ return cache_location[0]; }
	inline int getFirstBlock()	{ return cache_location[1]; }
	inline int getSecondBlock()	{ return cache_location[2]; }
	inline int getThirdBlock()	{ return cache_location[3]; }
	int getBlock(int i);

	inline void setFileSize(int FileSize)		{ cache_location[0] = FileSize; }
	inline void setFirstBlock(int BlockNumber)	{ cache_location[1] = BlockNumber; }
	inline void setSecondBlock(int BlockNumber)	{ cache_location[2] = BlockNumber; }
	inline void setThirdBlock(int BlockNumber)	{ cache_location[3] = BlockNumber; }
	int setBlock(int i, int BlockNumber);

private:
	int descriptorIndex;
	int* cache_location;
};

#endif//FILEDESCRIPTOR_HPP