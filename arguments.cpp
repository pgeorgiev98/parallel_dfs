#include "arguments.h"

#include <iostream>
#include <string>
#include <sstream>
#include <functional>
#include <cstdlib>

#include <getopt.h>

using namespace std;

template<typename T, typename Func>
bool argStringTo(const char *str, T &value, Func isValid, const char *errorMessage)
{
	istringstream in(str);
	in >> value;
	if (!in || !isValid(value))
		cerr << errorMessage << endl;
	return bool(in);
}

template<typename T>
bool argStringTo(const char *str, T &value, const char *errorMessage)
{
	return argStringTo(str, value, [](T) { return true; }, errorMessage);
}

template<>
bool argStringTo<string>(const char *str, string &value, const char *errorMessage)
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
	return true;
}

Arguments Arguments::parseArguments(int argc, char **argv)
{
	Arguments args;

	for (int r; (r = getopt(argc, argv, "t:n:i:o:q")) != -1; ) {
		switch (r) {
		case '?':
			exit(1);

		case 't':
			if (!argStringTo(optarg, args.threadCount,
						[](int t) { return t > 0; },
						"Invalid argument to -t"))
				exit(1);
			break;

		case 'n':
			if (!argStringTo(optarg, args.randomGraphSize,
						[](int n) { return n > 0; },
						"Invalid argument to -n"))
				exit(1);
			break;

		case 'i':
			if (!argStringTo(optarg, args.inputFile,
						"Invalid argument to -i"))
				exit(1);
			break;

		case 'o':
			if (!argStringTo(optarg, args.outputFile,
						"Invalid argument to -o"))
				exit(1);
			break;

		case 'q':
			args.quiet = true;
			break;

		default:
			exit(2);
		}
	}

	if (!args.verify()) {
		exit(1);
	}

	return args;
}
