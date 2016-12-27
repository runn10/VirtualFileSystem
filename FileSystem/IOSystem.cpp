#include <string.h>
#include <iostream>
#include <fstream>

#include "IOSystem.h"
#include "Errors.h"

IOSystem::IOSystem()
{
	ldisk = new char*[L];
	for (char** p = ldisk; p < ldisk + L; p++) 
		*p = new char[B];

	//Initialize disk to zeroes.
	for (int i = 0; i < L; i++)
	for (int j = 0; j < B; j++)
		ldisk[i][j] = 0;
}

IOSystem::IOSystem(const IOSystem& copy)
{
	ldisk = new char*[L];
	for (char** p = ldisk; p < ldisk + L; p++)
		*p = new char[B];

	//Initialize disk to copy copy
	for (int i = 0; i < L; i++)
	for (int j = 0; j < B; j++)
		ldisk[i][j] = copy.ldisk[i][j];
}

IOSystem& IOSystem::operator=(const IOSystem& copy)
{
	if (this !=& copy)
	{
		//Initialize disk to copy a.
		for (int i = 0; i < L; i++)
		for (int j = 0; j < B; j++)
			ldisk[i][j] = copy.ldisk[i][j];
	}
	return *this;
}

IOSystem::~IOSystem()
{
	for (char** p = ldisk; p < ldisk + L; p++)
		delete[] *p;
	delete[] ldisk;
}

void IOSystem::read_block(int i, char *p)
{
	std::memcpy(p, ldisk[i], this->B);
}

void IOSystem::write_block(int i, char *p)
{
	std::memcpy(ldisk[i], p, this->B);
}

void IOSystem::save(std::string filename)
{
	std::ofstream file(filename);
	for (int i = 0; i < this->L; i++)
	{
		for (int j = 0; j < this->B; j++) file << (int)ldisk[i][j] << " ";
		file << '\n';
	}
	file.close(); 
}

void IOSystem::load(std::string filename)
{
	std::ifstream file(filename);
	if (!file.good())
	{
		file.close();
		throw EXCEPTION.COULD_NOT_LOAD_FILE;
	}

	for (int i = 0; i < this->L; i++)
	{
		for (int j = 0; j < this->B; j++)
		{
			int num = 0;
			file >> num;
			ldisk[i][j] = (char)num;
		}
	}
	file.close();
}

void IOSystem::diskdump(int start, int end)
{
	for (int i = start; i < end; i++)
	{
		for (int j = 0; j < B; j++) 
			std::cout << (int)ldisk[i][j];
		std::cout << std::endl;
	}
}