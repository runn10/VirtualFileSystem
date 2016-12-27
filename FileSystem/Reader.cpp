
#include "TEST.h"
#include "Errors.h"
#include "IOSystem.h"
#include "Reader.h"
#include "FileSystem.h"

int Reader::Reads(std::string inputFile)
{
	std::ifstream in(inputFile);
	int check = 0;
	if (in.is_open())
	{
		static const std::string commands[]{"in", "op", "cl", "cr", "de", "rd", "wr", "sk", "sv", "dr", "runtests"};
		IOSystem io;
		FileSystem* fs = nullptr;
		std::string s1 = "";		std::string s2 = "";
		std::ofstream o("G:/51035780.txt");
		while (!in.eof())
		{
			std::getline(in, s1);
			bool isValidCommand = std::find(std::begin(commands), std::end(commands), inputFile) != std::end(commands);
			if (fs == nullptr && isValidCommand && s1 != "runtests")
				ERROR.printError("Disk has not been initialized yet.\n", o);

			else if (s1.substr(0, 2) == "in")
			{
				if (fs == nullptr) delete fs;
				fs = new FileSystem(io, s2);

				if (fs != NULL)
				{
					if (fs->getDiskStatus() == INITIALIZED)		o << "disk initialized\n";
					else if (fs->getDiskStatus() == RESTORED)	o << "disk restored\n";
				}
			}

			else if (s1.substr(0, 2) == "cr")
			{
				std::string fileName = s1.substr(3);
				int test = fs->create(fileName);

				if (test == ERROR.NAME_TOO_LONG)			ERROR.printError("error ", o);
				else if (test == ERROR.DUPLICATE_FILE)		ERROR.printError("error ", o);
				else if (test == ERROR.NO_SPACE_ON_DISK)	ERROR.printError("error ", o);
				else										
					o << fileName << " created\n";
			}

			else if (s1.substr(0, 2) == "op")
			{
				std::string fileName = s1.substr(3);
				int test = fs->open(fileName);

				if (test == ERROR.NAME_TOO_LONG)			ERROR.printError("error ", o);
				else if (test == ERROR.NO_FREE_OFT_ENTRY)	ERROR.printError("error ", o);
				else if (test == ERROR.FILE_NOT_FOUND)		ERROR.printError("error ", o);
				else if (test == ERROR.FILE_ALREADY_OPEN)	ERROR.printError("error ", o);
				else										
					o << fileName << " opened " << test << "\n";
			}

			else if (s1.substr(0, 2) == "sv")
			{
				s2 = s1.substr(3);
				std::string txtFile = s1.substr(3);

				fs->save(txtFile);
				o << "disk saved\n";
			}


			else if (s1.substr(0, 2) == "dr")
			{
				fs->directory(o);
			}

			else if (s1.substr(0, 2) == "rd")
			{

				int index, count;
				index = std::stoi(s1.substr(2, 4));
				count = std::stoi(s1.substr(4));

				char * mem = new char[count + 1];
				int bytesRead = fs->read(index, mem, count);

				if (bytesRead == ERROR.FILE_IS_NOT_OPEN)	ERROR.printError("error ", o);
				else if (bytesRead == ERROR.END_OF_FILE)	ERROR.printError("error ", o);
				else
				{
					mem[bytesRead] = '\0';
					o << mem << std::endl;
					delete[] mem;
				}
			}

			else if (s1.substr(0, 2) == "sk")
			{
				int index, pos;
				index = std::stoi(s1.substr(2, 4));
				pos = std::stoi(s1.substr(4));
				int test = fs->lseek(index, pos);

				if (test == ERROR.FILE_IS_NOT_OPEN)				ERROR.printError("error ", o);
				else if (test == ERROR.POSITION_OUT_OF_BOUNDS)	ERROR.printError("error ", o);
				else											
					o << "position is " << pos << "\n";
			}

			else if (s1.substr(0, 2) == "wr")
			{
				int index, count;
				char value;
				value = s1.substr(5)[0];
				index = std::stoi(s1.substr(2, 4));
				count = std::stoi(s1.substr(6));
				int result = fs->write(index, value, count);

				if (result == ERROR.FILE_IS_NOT_OPEN)			ERROR.printError("error ", o);
				else if (result == ERROR.MAX_FILESIZE_REACHED)	ERROR.printError("error ", o);
				else											
					o << count << " bytes written\n";

			}

			else if (s1.substr(0, 2) == "de")
			{
				std::string txtFile = s1.substr(3);
				int test = fs->destroy(txtFile);

				if (test == ERROR.NAME_TOO_LONG)			ERROR.printError("error ", o);
				else if (test == ERROR.FILE_NOT_FOUND)		ERROR.printError("error ", o);
				else										
					o << txtFile << " destroyed\n";
			}

			else if (s1.substr(0, 2) == "cl")
			{
				int index;
				index = std::stoi(s1.substr(2, 4));
				if (index == 0)
				{
					ERROR.printError("error ", o);
				}
				else
				{
					int result = fs->close(index);
					if (result == ERROR.FILE_IS_NOT_OPEN)	ERROR.printError("error ", o);
					else									
						o << index << " closed\n";
				}
			}

			else if (s1 == "runtests")
			{
				TEST.TestAll(true);
			}

			else
			{
				o << "\n";
				continue;
			}
		}	
		in.close();
		o.close();
	}

	return 1;
}