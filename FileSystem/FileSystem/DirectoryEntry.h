#ifndef DIRECTORYENTRY_HPP
#define DIRECTORYENTRY_HPP

#include <string>

class DirectoryEntry
{
public:
	DirectoryEntry();
	DirectoryEntry(int position, char (&contents)[8]);

	inline int getPosition() { return position; }
	inline std::string getName() { return name; }
	inline int getDescriptorIndex() { return fileDescriptorIndex; }

	void setName(std::string name);
	void setDescriptorIndex(int descriptorIndex);

private:
	int position;
	std::string name;
	int fileDescriptorIndex;
};

#endif//DIRECTORYENTRY_HPP