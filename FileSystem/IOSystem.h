#ifndef IOSYSTEM_HPP
#define IOSYSTEM_HPP

#include <string>

class IOSystem
{
public:
	static const int L = 64;
	static const int B = 64;

	IOSystem();
	IOSystem(const IOSystem& a);
	IOSystem& operator=(const IOSystem& a);
	~IOSystem();

	// This copies the logical block ldisk[i] into main memory
	// starting at the location specified by the pointer p. 
	// The number of characters copied corresponds to the size 
	// of the block length, B.
	// TRANSLATION: Copy ldisk[i][0] through ldisk[i][B-1] to p;
	void read_block(int i, char *p);
	
	// This copies the number of character corresponding to the
	// block length, B, from main memory starting at the location 
	// specified by the pointer p, into the logical block ldisk[i].
	// TRANSLATION: Copy p[0] through p[B-1] to L[i];
	void write_block(int i, char *p);

	// save the contents of ldisk into a file
	// containing L lines of B bytes.
	void save(std::string filename);

	// load the contents of ldisk from a file
	// formatted in L lines of B bytes.
	// reinitialize the OFT?
	//	-figure out first block from directory file
	//	-need to read in descriptior table
	void load(std::string filename);


	void diskdump(int start, int end);
	
private:

	// The physical disk is a two-dimensional structure consisting of cylinders,
	// tracks within cylinders, sectors within tracks, and bytes within sectors. 
	// The task of the I/O system is to hide the two-dimensional organization 
	// by presenting the disk as a linear sequence of logical blocks, numbered 
	// 0 through L – 1, where L is the total number of blocks on the physical disk.

	// We will model the disk as a character array ldisk[L][B], where L is the number
	// of logical blocks and B is the block length, i.e., the number of bytes per block. 
	// The task of the I/O system is to accept a logical block number from the file 
	// system and to read or write the corresponding block into a memory area specified 
	// by the command.
	char** ldisk;
};

#endif//IOSYSTEM_HPP