#include <iostream>
#include "IOSystem.h"
#include "FileSystem.h"
#include <string>
#include "Errors.h"
#include "TEST.h"

bool TESTS::TestCreate()
{
	IOSystem io;
	FileSystem fs(io, "storage");
	if (fs.create("name") != SUCCESS) return false;

	int count = 0;
	DirectoryEntry* entries = fs.getDirectoryEntries(count);
	if (count != 1) { delete[] entries; return false; }
	if (entries[0].getName() != "name")  { delete[] entries; return false; }
	FileDescriptor fd = fs.getDescriptorTable()->getFileDescriptor(entries[0].getDescriptorIndex());
	if (fd.getFileSize() != 0) { delete[] entries; return false; }
	delete[] entries;
	return true;
}

bool TESTS::TestDestroy()
{
	IOSystem io;
	FileSystem fs(io, "storage");
	if (fs.create("keep") != SUCCESS) return false;
	if (fs.create("rmve") != SUCCESS) return false;
	if (fs.destroy("rmve") != SUCCESS) return false;

	int count = 0;
	DirectoryEntry* entries = fs.getDirectoryEntries(count);
	if (count != 1)  { delete[] entries; return false; }
	if (entries[0].getName() != "keep")  { delete[] entries; return false; }

	delete[] entries;
	return true;
}

bool TESTS::TestOpen()
{
	IOSystem io;
	FileSystem fs(io, "storage");
	if (fs.create("foo") != SUCCESS) return false;
	if (fs.open("foo") != 1) return false;
	return true;
}

bool TESTS::TestWrite()
{
	IOSystem io;
	FileSystem fs(io, "storage");
	if (fs.create("name") != SUCCESS) return false;
	int index = fs.open("name");
	if (index != 1) return false;
	if (fs.write(index, 'a', 5) != 5) return false;

	int count = 0;
	DirectoryEntry* entries = fs.getDirectoryEntries(count);
	FileDescriptor fd = fs.getDescriptorTable()->getFileDescriptor(entries[0].getDescriptorIndex());
	if (fd.getFileSize() != 5) { delete[] entries; return false; }
	
	delete[] entries;
	return true;
}

bool TESTS::TestRead()
{
	IOSystem io;
	FileSystem fs(io, "storage");
	if (fs.create("name") != SUCCESS) return false;
	int index = fs.open("name");
	if (index != 1) return false;
	if (fs.write(index, 'a', 5) != 5) return false;

	char readArray[5];
	fs.lseek(index, 0);
	if (fs.read(index, readArray, 5) != 5) return false;
	for (int i = 0; i < 5; i++)
	if (readArray[i] != 'a') return false;

	return true;
}

bool TESTS::TestWriteAndRead_Large()
{
	IOSystem io;
	FileSystem fs(io, "storage");
	if (fs.create("name") != SUCCESS) return false;
	int index = fs.open("name");
	if (index != 1) return false;
	if (fs.write(index, 'a', 192) != 192) return false;

	char readArray[192];
	fs.lseek(index, 0);
	if (fs.read(index, readArray, 192) != 192) return false;
	for (int i = 0; i < 192; i++)
	if (readArray[i] != 'a') return false;

	return true;
}

//////////////////////////////////////////////
// Test ERRORS. Make sure errors are thrown.//
//////////////////////////////////////////////

bool TESTS::Test_NoSpaceOnDisk()
{
	IOSystem io;
	FileSystem fs(io, "storage");

	for (int i = 0; i < 23; i++)
	{
		std::string filename = std::to_string(i);
		if (fs.create(filename) != 0) return false;
	}
	if (fs.create("24") != ERROR.NO_SPACE_ON_DISK) return false;
	return true;
}

bool TESTS::Test_DuplicateFile()
{
	IOSystem io;
	FileSystem fs(io, "storage");

	if (fs.create("one") != SUCCESS) return false;
	if (fs.create("one") != ERROR.DUPLICATE_FILE) return false;
	return true;
}

bool TESTS::Test_FileNotFound()
{
	IOSystem io;
	FileSystem fs(io, "storage");

	if (fs.open("one") != ERROR.FILE_NOT_FOUND) return false;
	if (fs.destroy("one") != ERROR.FILE_NOT_FOUND) return false;
	return true;
}

bool TESTS::Test_FileIsNotOpen()
{
	IOSystem io;
	FileSystem fs(io, "storage");

	char mem[5];
	if (fs.read(1, mem, 5) != ERROR.FILE_IS_NOT_OPEN) return false;
	if (fs.write(1, 'a', 5) != ERROR.FILE_IS_NOT_OPEN) return false;
	if (fs.lseek(1, 0) != ERROR.FILE_IS_NOT_OPEN) return false;
	if (fs.close(1) != ERROR.FILE_IS_NOT_OPEN) return false;
	return true;
}

bool TESTS::Test_EndOfFile()
{
	IOSystem io;
	FileSystem fs(io, "storage");

	char mem[6];
	if (fs.create("file") != SUCCESS) return false;
	if (fs.open("file") != 1) return false;
	if (fs.write(1, 'a', 5) != 5) return false;
	if (fs.read(1, mem, 6) != ERROR.END_OF_FILE) return false;
	return true;
}

bool TESTS::Test_PositionOutOfBounds()
{
	IOSystem io;
	FileSystem fs(io, "storage");
	
	if (fs.create("file") != SUCCESS) return false;
	if (fs.open("file") != 1) return false;
	if (fs.write(1, 'a', 5) != 5) return false;
	if (fs.lseek(1, -1) != ERROR.POSITION_OUT_OF_BOUNDS) return false;
	if (fs.lseek(1, 6) != ERROR.POSITION_OUT_OF_BOUNDS) return false;
	return true;
}

bool TESTS::Test_MaxFileSizeReached()
{
	IOSystem io;
	FileSystem fs(io, "storage");

	if (fs.create("file") != SUCCESS) return false;
	if (fs.open("file") != 1) return false;
	if (fs.write(1, 'a', 193) != ERROR.MAX_FILESIZE_REACHED) return false;
	return true;
}

bool TESTS::Test_NameTooLong()
{
	IOSystem io;
	FileSystem fs(io, "storage");

	if (fs.create("TooLong") != ERROR.NAME_TOO_LONG) return false;
	if (fs.destroy("TooLong") != ERROR.NAME_TOO_LONG) return false;
	if (fs.open("TooLong") != ERROR.NAME_TOO_LONG) return false;
	return true;
}

bool TESTS::Test_NoFreeOFTEntry()
{
	IOSystem io;
	FileSystem fs(io, "storage");

	if (fs.create("1") != SUCCESS) return false;
	if (fs.create("2") != SUCCESS) return false;
	if (fs.create("3") != SUCCESS) return false;
	if (fs.create("4") != SUCCESS) return false;
	if (fs.open("1") != 1) return false;
	if (fs.open("2") != 2) return false;
	if (fs.open("3") != 3) return false;
	if (fs.open("4") != ERROR.NO_FREE_OFT_ENTRY) return false;
	return true;
}

///////////////////////
// Run all the tests.//
///////////////////////

bool TESTS::TestAll(bool verbose)
{
	bool Passed = true;

	int TestCount = 15;
	typedef bool(*TestFunction)();
	TestFunction testFunctions[] =
	{
		TestCreate, TestDestroy, TestOpen, TestWrite, TestRead, TestWriteAndRead_Large,
		Test_NoSpaceOnDisk, Test_DuplicateFile, Test_FileNotFound, Test_FileIsNotOpen,
		Test_EndOfFile, Test_PositionOutOfBounds, Test_MaxFileSizeReached, Test_NameTooLong,
		Test_NoFreeOFTEntry
	};
	std::string testFunctionNames[] =
	{
		"TestCreate", "TestDestroy", "TestOpen", "TestWrite", "TestRead", "TestWriteAndRead_Large",
		"Test_NoSpaceOnDisk", "Test_DuplicateFile", "Test_FileNotFound", "Test_FileIsNotOpen",
		"Test_EndOfFile", "Test_PositionOutOfBounds", "Test_MaxFileSizeReached", "Test_NameTooLong",
		"Test_NoFreeOFTEntry"
	};

	//Run all the tests!
	for (int i = 0; i < TestCount; i++)
	if (!testFunctions[i]())
	{
		if (verbose) std::cout << testFunctionNames[i] << "() Failed." << std::endl;
		Passed = false;
	}

	// Did they all pass?
	if (Passed && verbose)
		std::cout << "All Test Cases Passed." << std::endl;

	return Passed;
}
