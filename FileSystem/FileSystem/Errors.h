#ifndef ERRORS_HPP
#define ERRORS_HPP

#include <string>
#include <iostream>

class ERRORS
{
public:
	inline static void printError(std::string error) { std::cout << error << std::endl; }
	static const int NO_SPACE_ON_DISK		= -1; // Create
	static const int DUPLICATE_FILE			= -2; // Create
	static const int FILE_NOT_FOUND			= -1; // Destroy, Open
	static const int FILE_IS_NOT_OPEN		= -1; // Read, Write, Lseek, Close
	static const int END_OF_FILE			= -2; // Read
	static const int POSITION_OUT_OF_BOUNDS = -2; // Lseek
	static const int MAX_FILESIZE_REACHED	= -2; // Write
	static const int NAME_TOO_LONG			= -3; // Create, Destroy, Open
	static const int NO_FREE_OFT_ENTRY		= -2; // Open
};

class EXCEPTIONS
{
public:
	static const char NO_FREE_FILE_DESCRIPTORS	= 'a';
	static const char NO_ROOM_IN_DIRECTORY		= 'b';
	static const char NAME_TOO_LONG				= 'c';
	static const char FILE_NOT_FOUND			= 'd';
	static const char INDEX_OUT_OF_RANGE		= 'e';
	static const char NO_FREE_OFT_ENTRY			= 'f';
	static const char CONTROL_FLOW_BUG			= 'g';
	static const char NO_FREE_BLOCKS			= 'h';
	static const char COULD_NOT_LOAD_FILE		= 'i';
};

static const int SUCCESS = 0;
static const ERRORS ERROR;
static const EXCEPTIONS EXCEPTION;



#endif//ERRORS_HPP