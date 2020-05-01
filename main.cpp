#include "arguments.h"

#include <iostream>
using namespace std;

int main(int argc, char **argv)
{
	Arguments args = Arguments::parseArguments(argc, argv);

	cout << "Threads: " << args.threadCount << endl;
	cout << "Graph size: " << args.randomGraphSize << endl;
	cout << "Input file: " << args.inputFile << endl;
	cout << "Output file: " << args.outputFile << endl;
	cout << "Quiet: " << boolalpha << args.quiet << endl;
	return 0;
}
