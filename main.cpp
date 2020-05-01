#include "arguments.h"
#include "graph.h"

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

	Graph g;
	if (args.randomGraphSize) {
		g = std::move(Graph::randomGraph(args.randomGraphSize));
	} else {
		auto r = Graph::fromFile(args.inputFile);
		if (!r) {
			cerr << r.error() << endl;
			return 1;
		}
		g = std::move(*r);
	}

	for (int i = 0; i < g.nodeCount; ++i) {
		for (int j = 0; j < g.nodeCount; ++j) {
			cout << int(g.relations[i][j]) << ' ';
		}
		cout << endl;
	}

	return 0;
}
