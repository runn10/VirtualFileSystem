#ifndef OPENFILE_HPP
#define OPENFILE_HPP

struct OpenFile
{
	static const int BufferSize = 64;
	char RW_Buffer[BufferSize];
	int currentPosition;
	int fileDescriptorIndex;
	OpenFile(int fileDescriptorIndex) : 
		currentPosition(0), 
		fileDescriptorIndex(fileDescriptorIndex) {}
};

#endif