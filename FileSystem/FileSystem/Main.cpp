#include <iostream>
#include <stdio.h>
#include "FileSystem.h"
#include "IOSystem.h"
#include "Errors.h"
#include <algorithm>
#include "TEST.h"

int main()
{
	IOSystem io;
	FileSystem* fs = nullptr;

	std::string Command;
	static const std::string commands[]{"in", "op", "cl", "cr", "de", "rd", "wr", "sk", "sv", "dr", "runtests"};

	while (std::cin >> Command)
	{
		bool isValidCommand = std::find(std::begin(commands), std::end(commands), Command) != std::end(commands);
		if (fs == nullptr && isValidCommand && Command != "in" && Command != "runtests")
		{
			std::string dummy;
			std::getline(std::cin, dummy);
			ERROR.printError("Disk has not yet been initialized.");
		}
		else if (Command == "in")
		{
			if (fs == nullptr) delete fs;

			int dummy;
			std::cin >> dummy >> dummy >> dummy >> dummy;
			std::string txtFile;
			std::cin >> txtFile;
			fs = new FileSystem(io, txtFile);

			if (fs->getDiskStatus() == INITIALIZED)		std::cout << "disk initialized." << std::endl;
			else if (fs->getDiskStatus() == RESTORED)	std::cout << "disk restored." << std::endl;
		}
		else if (Command == "sv")
		{
			std::string txtFile;
			std::cin >> txtFile;

			fs->save(txtFile);
			std::cout << "disk saved." << std::endl;
		}
		else if (Command == "cr")
		{
			std::string fileName;
			std::cin >> fileName;
			int test = fs->create(fileName);

			if (test == ERROR.NAME_TOO_LONG)			ERROR.printError("file not created, file name too long.");
			else if (test == ERROR.DUPLICATE_FILE)		ERROR.printError("file not created, there is file with a duplicate name.");
			else if (test == ERROR.NO_SPACE_ON_DISK)	ERROR.printError("file not created, no space on disk.");
			else										std::cout << "file " << fileName << " created." << std::endl;
		}
		else if (Command == "de")
		{
			std::string fileName;
			std::cin >> fileName;
			int test = fs->destroy(fileName);

			if (test == ERROR.NAME_TOO_LONG)			ERROR.printError("file not destroyed, file name too long.");
			else if (test == ERROR.FILE_NOT_FOUND)		ERROR.printError("file not destroyed, could not find the file specified.");
			else										std::cout << "file " << fileName << " destroyed." << std::endl;
		}
		else if (Command == "op")
		{
			std::string fileName;
			std::cin >> fileName;
			int test = fs->open(fileName);

			if (test == ERROR.NAME_TOO_LONG)			ERROR.printError("file could not be opened, file name too long.");
			else if (test == ERROR.NO_FREE_OFT_ENTRY)	ERROR.printError("file could not be opened, there are no free Open File Table entries.");
			else if (test == ERROR.FILE_NOT_FOUND)		ERROR.printError("file could not be opened, could not find the file name specified.");
			else										std::cout << "file " << fileName << " opened, index = " << test << std::endl;
		}
		else if (Command == "cl")
		{
			int index;
			std::cin >> index;
			if (index == 0)
			{
				ERROR.printError("you cannot close the directory file");
			}
			else
			{
				int result = fs->close(index);
				if (result == ERROR.FILE_IS_NOT_OPEN)	ERROR.printError("file could not be closed, it is not open");
				else									std::cout << "file at index " << index << " closed." << std::endl;
			}
		}
		else if (Command == "wr")
		{
			int index, count;
			char value;
			std::cin >> index;
			std::cin >> value;
			std::cin >> count;
			int result = fs->write(index, value, count);

			if (result == ERROR.FILE_IS_NOT_OPEN)			ERROR.printError("cannot write, file is not open.");
			else if (result == ERROR.MAX_FILESIZE_REACHED)	ERROR.printError("maximum file size has been reached.");
			else											std::cout << count << " bytes written." << std::endl;

		}
		else if (Command == "sk")
		{
			int index, pos;
			std::cin >> index;
			std::cin >> pos;
			int test = fs->lseek(index, pos);

			if (test == ERROR.FILE_IS_NOT_OPEN)				ERROR.printError("could not seek, file is not open.");
			else if (test == ERROR.POSITION_OUT_OF_BOUNDS)	ERROR.printError("could not seek, position is out of bounds.");
			else											std::cout << "current position is " << pos << std::endl;
		}
		else if (Command == "rd")
		{

			int index, count;
			std::cin >> index;
			std::cin >> count;

			char * mem = new char[count + 1];
			int bytesRead = fs->read(index, mem, count);

			if (bytesRead == ERROR.FILE_IS_NOT_OPEN)	ERROR.printError("cannot read from file because it is not open.");
			else if (bytesRead == ERROR.END_OF_FILE)	ERROR.printError("end of file, cannot read from file.");
			else
			{
				mem[bytesRead] = '\0';
				std::cout << bytesRead << " bytes read: " << mem << std::endl;
				delete[] mem;
			}
		}
		else if (Command == "dr")
		{
			fs->directory();
		}
		else if (Command == "runtests")
		{
			TEST.TestAll(true);
		}
		else
		{
			std::string RestOfLine;
			std::getline(std::cin, RestOfLine);
			ERROR.printError("INVALID COMMAND, please try again.");
		}
	}

	std::system("pause");
	return 0;
}