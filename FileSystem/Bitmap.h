#ifndef BITMAP_HPP
#define BITMAP_HPP

class Bitmap
{
public:
	Bitmap(char* cache_location, int K, int L);
	bool isBlockFree(int blockNumber);
	void setBlock(int blockNumber);
	void freeBlock(int blockNumber);
	int findFreeBlock();

private:
	char* cache_location;
	int K, L;
};

#endif//BITMAP_HPP