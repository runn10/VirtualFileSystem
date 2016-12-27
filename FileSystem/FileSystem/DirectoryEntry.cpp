#include "DirectoryEntry.h"
#include "Errors.h"
#include <string>

DirectoryEntry::DirectoryEntry() :
	position(0),
	name(""),
	fileDescriptorIndex(0) { }

DirectoryEntry::DirectoryEntry(int position, char(&contents)[8]) :
	position(position),
	name(contents, contents + 4),
	fileDescriptorIndex(*(const int *)(&contents[4])) { }

void DirectoryEntry::setName(std::string name)
{
	if (name.length() > 4)
		throw EXCEPTION.NAME_TOO_LONG;
	this->name = name;
}

void DirectoryEntry::setDescriptorIndex(int descriptorIndex)
{
	fileDescriptorIndex = descriptorIndex;
}