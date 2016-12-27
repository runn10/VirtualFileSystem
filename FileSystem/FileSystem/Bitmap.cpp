#include "Bitmap.h"
#include "Errors.h"
#include <iostream>

Bitmap::Bitmap(char* cache_location, int K, int L) :
	cache_location(cache_location),
	K(K), L(L) {}

bool Bitmap::isBlockFree(int blockNumber)
{
	if (blockNumber < 0 || blockNumber >= L)
		throw EXCEPTION.INDEX_OUT_OF_RANGE;
	int byte = blockNumber / 8;
	int bit = blockNumber % 8;
	return (cache_location[byte] & (1 << bit)) == 0;
}

void Bitmap::setBlock(int blockNumber)
{
	if (blockNumber < 0 || blockNumber >= L)
		throw EXCEPTION.INDEX_OUT_OF_RANGE;
	int byte = blockNumber / 8;
	int bit = blockNumber % 8;
	cache_location[byte] |= (1 << bit);
}

void Bitmap::freeBlock(int blockNumber)
{
	if (blockNumber < 0 || blockNumber >= L)
		throw EXCEPTION.INDEX_OUT_OF_RANGE;
	int byte = blockNumber / 8;
	int bit = blockNumber % 8;
	cache_location[byte] &= ~(1 << bit);
}

int Bitmap::findFreeBlock()
{
	for (int i = K; i < L; i++)
		if (isBlockFree(i)) 
			return i;
	throw EXCEPTION.NO_FREE_BLOCKS;
}