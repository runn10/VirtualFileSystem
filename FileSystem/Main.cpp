#include <iostream>
#include <stdio.h>
#include <algorithm>


#include "FileSystem.h"
#include "IOSystem.h"
#include "Errors.h"
#include "TEST.h"
#include "Reader.h"

int main()
{
	Reader PresentationShell;

	while (true)
	{
		int decider = PresentationShell.Reads("G:/input.txt");
		if (decider == 1)
			break;
	}

	std::system("pause");
	return 0;
}