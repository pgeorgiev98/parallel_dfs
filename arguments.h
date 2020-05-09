#ifndef ARGUMENTS_H_INCLUDED
#define ARGUMENTS_H_INCLUDED

#include <string>
#include <vector>
#include <iostream>

struct Arguments
{
	std::vector<int> threadCount = {1};
	int randomGraphSize = 0;
	int passes = 1;
	std::string inputFile;
	std::string outputFile;
	std::string randomGraphOutputFile;
	std::string outputCsvFile;
	bool quiet = false;

	bool verify() const;
	static Arguments parseArguments(int argc, char **argv);
	static void printUsage(const char *arg0, std::ostream &out);
};

#endif
