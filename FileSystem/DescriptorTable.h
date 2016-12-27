#ifndef DESCRIPTORTABLE_HPP
#define DESCRIPTORTABLE_HPP

#include "IOSystem.h"
#include "Bitmap.h"
#include "FileDescriptor.h"

class DescriptorTable 
{
public:
	static const int K = 7;
	static const int DescriptorCount = 24;

	DescriptorTable(IOSystem& io);
	DescriptorTable(const DescriptorTable& a);
	DescriptorTable& operator=(const DescriptorTable& a);
	~DescriptorTable();
	void UpdateLdisk();

	Bitmap getBitmap();
	FileDescriptor findFreeFileDescriptor();
	FileDescriptor getFileDescriptor(int descriptorIndex);
	void freeFileDescriptor(FileDescriptor fileDescriptor);

private:
	IOSystem& io;	// A Reference to the IOSystem
	char** cache;	// Descriptor Table (in memory).
};

#endif//DESCRIPTORTABLE_HPP