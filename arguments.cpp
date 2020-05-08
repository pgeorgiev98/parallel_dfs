#include "arguments.h"

#include <iostream>
#include <string>
#include <sstream>
#include <functional>
#include <cstdlib>

#include <getopt.h>

using namespace std;

template<typename T, typename Func>
bool argStringTo(const char *str, T &value, Func isValid, const string &errorMessage)
{
	istringstream in(str);
	in >> value;
	if (!in || !isValid(value))
		cerr << errorMessage << endl;
	return bool(in);
}

template<typename T>
bool argStringTo(const char *str, T &value, const string &errorMessage)
{
	return argStringTo(str, value, [](T) { return true; }, errorMessage);
}

template<>
bool argStringTo<string>(const char *str, string &value, const string &errorMessage)
{
	value = str;
	return !value.empty();
}


bool Arguments::verify() const
{
	if (threadCount == 0) {
		cerr << "Thread count expected" << endl;
		return false;
	}
	if ((randomGraphSize == 0) == inputFile.empty()) {
		cerr << "Either specify graph size or input file" << endl;
		return false;
	}
	if (!randomGraphOutputFile.empty() && randomGraphSize == 0) {
		cerr << "Option -s requires -n" << endl;
		return false;
	}
	return true;
}

static int parseArgumentsInternal(int argc, char **argv, Arguments &args)
{
	for (int r; (r = getopt(argc, argv, "ht:n:p:i:o:s:q")) != -1; ) {
		string err = string("Invalid argument to ") + char(r);
		switch (r) {
		case '?':
			return 1;

		case 'h':
			return -1;

		case 't':
			if (!argStringTo(optarg, args.threadCount,
						[](int t) { return t > 0; }, err))
				return 1;
			break;

		case 'n':
			if (!argStringTo(optarg, args.randomGraphSize,
						[](int n) { return n > 0; }, err))
				return 1;
			break;

		case 'p':
			if (!argStringTo(optarg, args.passes,
						[](int n) { return n > 0; }, err))
				return 1;
			break;

		case 'i':
			if (!argStringTo(optarg, args.inputFile, err))
				return 1;
			break;

		case 'o':
			if (!argStringTo(optarg, args.outputFile, err))
				return 1;
			break;

		case 's':
			if (!argStringTo(optarg, args.randomGraphOutputFile, err))
				return 1;
			break;

		case 'q':
			args.quiet = true;
			break;

		default:
			return 2;
		}
	}

	return 0;
}

Arguments Arguments::parseArguments(int argc, char **argv)
{
	Arguments args;

	int e = parseArgumentsInternal(argc, argv, args);
	if (e != 0) {
		if (e == -1) {
			printUsage(argv[0], cout);
			exit(0);
		} else {
			printUsage(argv[0], cerr);
			exit(e);
		}
	}

	if (!args.verify()) {
		printUsage(argv[0], cerr);
		exit(1);
	}

	return args;
}

void Arguments::printUsage(const char *arg0, ostream &out)
{
	out << "Usage: " << arg0 << " [OPTIONS]" << endl
		<< endl
		<< "  -h         Show this message" << endl
		<< "  -t num     Run the algorithm on num threads" << endl
		<< "  -n num     Operate on a randomly generated graph with num nodes" << endl
		<< "  -p num     Traverse the graph num times" << endl
		<< "  -i file    Operate on a graph, described in file" << endl
		<< "  -o file    Write algorithm result in file" << endl
		<< "  -s file    Save the randomly generated graph to file (needs -n)" << endl
		<< "  -q         Lower the noise" << endl;
}
