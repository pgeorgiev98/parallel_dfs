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

static vector<string> splitString(const string &str, char delimiter)
{
	vector<string> v;
	string curr;
	for (auto c : str) {
		if (c == delimiter) {
			v.push_back(curr);
			curr.clear();
		} else {
			curr.push_back(c);
		}
	}
	if (!curr.empty())
		v.push_back(curr);
	return v;
}

template<>
bool argStringTo<vector<int>>(const char *str, vector<int> &value, const string &errorMessage)
{
	vector<int> result;
	string s(str);
	try {
		for (const string &value : splitString(s, ',')) {
			auto range = splitString(value, '-');
			if (range.size() == 1) {
				istringstream in(value);
				int i;
				in >> i;
				if (!in || i <= 0)
					throw errorMessage;
				result.push_back(i);
			} else if (range.size() == 2) {
				istringstream in1(range[0]), in2(range[1]);
				int a, b;
				in1 >> a;
				in2 >> b;
				if (!in1 || !in2 || a >= b || a <= 0 || b <= 0)
					throw errorMessage;
				for (int i = a; i <= b; ++i)
					result.push_back(i);
			} else {
				throw errorMessage;
			}
		}
	} catch (const string &err) {
		cerr << err << endl;
		return false;
	}

	value = result;
	return true;
}


bool Arguments::verify() const
{
	if ((randomGraphSize == 0) == inputFile.empty()) {
		cerr << "Either specify graph size or input file" << endl;
		return false;
	}
	if (!randomGraphOutputFile.empty() && randomGraphSize == 0) {
		cerr << "Option -s requires -n" << endl;
		return false;
	}
	if (randomGraphSize > 0 && randomGraphMaxEdgeCount > randomGraphSize) {
		cerr << "Invalid argument to -N" << endl;
		return false;
	}
	return true;
}

static int parseArgumentsInternal(int argc, char **argv, Arguments &args)
{
	for (int r; (r = getopt(argc, argv, "ht:n:N:p:i:o:s:l:q")) != -1; ) {
		string err = string("Invalid argument to ") + char(r);
		switch (r) {
		case '?':
			return 1;

		case 'h':
			return -1;

		case 't':
			if (!argStringTo(optarg, args.threadCount, err))
				return 1;
			break;

		case 'n':
			if (!argStringTo(optarg, args.randomGraphSize,
						[](int n) { return n > 0; }, err))
				return 1;
			break;

		case 'N':
			if (!argStringTo(optarg, args.randomGraphMaxEdgeCount,
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

		case 'l':
			if (!argStringTo(optarg, args.outputCsvFile, err))
				return 1;
			break;

		case 'q':
			args.quiet = true;
			break;

		default:
			return 2;
		}
	}

	if (args.randomGraphSize > 0 && args.randomGraphMaxEdgeCount == 0)
		args.randomGraphMaxEdgeCount = args.randomGraphSize;
	return 0;
}

Arguments Arguments::parseArguments(int argc, char **argv)
{
	Arguments args;

	for (int i = 0; i < argc; ++i) {
		args.commandLine += ' ';
		args.commandLine += argv[i];
	}

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
		<< "  -t num     Run the algorithm on num threads. Can be a comma" << endl
		<< "             separated list of numbers and ranges (See example)" << endl
		<< "  -n num     Operate on a randomly generated graph with num nodes" << endl
		<< "  -N num     The maximum number of edges begining from each node in" << endl
		<< "             the randomly generated graph" << endl
		<< "  -p num     Traverse the graph num times" << endl
		<< "  -i file    Operate on a graph, described in file" << endl
		<< "  -o file    Write algorithm result in file" << endl
		<< "  -s file    Save the randomly generated graph to file (needs -n)" << endl
		<< "  -l file    Save the run times to a csv file" << endl
		<< "  -q         Lower the noise" << endl
		<< endl << endl
		<< "Examples:" << endl
		<< endl
		<< "dfs -n 30000 -t1,2,4 -p 5     # Run on 30000 node graph with 1, 2 and 4" << endl
		<< "                              # threads with 5 passes each" << endl
		<< "dfs -i input.txt -t1-8 -p 3   # Run on graph from input.txt and 1, 2, 3," << endl
		<< "                              # 4, 5, 6, 7 and 8 cores with 3 passes each" << endl
		<< "dfs -n 10000 -s output.txt    # Generate random 10000 node graph and save it" << endl;
}
