#ifndef ARGUMENTS_H_INCLUDED
#define ARGUMENTS_H_INCLUDED

#include <string>

struct Arguments
{
	int threadCount = 0;
	int randomGraphSize = 0;
	std::string inputFile;
	std::string outputFile;
	bool quiet = false;

	bool verify() const;
	static Arguments parseArguments(int argc, char **argv);
};

#endif
