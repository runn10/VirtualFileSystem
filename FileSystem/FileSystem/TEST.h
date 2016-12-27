#ifndef TEST_HPP
#define TEST_HPP

class TESTS
{
public:
	static bool TestCreate();
	static bool TestDestroy();
	static bool TestOpen();
	static bool TestWrite();
	static bool TestRead();
	static bool TestWriteAndRead_Large();

	//////////////////////////////////////////////
	// Test ERRORS. Make sure errors are thrown.//
	//////////////////////////////////////////////

	static bool Test_NoSpaceOnDisk();
	static bool Test_DuplicateFile();
	static bool Test_FileNotFound();
	static bool Test_FileIsNotOpen();
	static bool Test_EndOfFile();
	static bool Test_PositionOutOfBounds();
	static bool Test_MaxFileSizeReached();
	static bool Test_NoFreeOFTEntry();
	static bool Test_NameTooLong();

	///////////////////
	// Run all Tests.//
	///////////////////
	static bool TestAll(bool verbose);
};

static const TESTS TEST;

#endif//TEST_HPP