#include "arguments.h"
#include "graph.h"
#include "log.h"

#include <iostream>
using namespace std;

int main(int argc, char **argv)
{
	Arguments args = Arguments::parseArguments(argc, argv);

	if (args.quiet)
		Logger::setMaximumLogLevel(Logger::LogLevel::Info);

	logDebug() << "Threads:" << args.threadCount;
	logDebug() << "Graph size:" << args.randomGraphSize;
	logDebug() << "Input file:" << args.inputFile;
	logDebug() << "Output file:" << args.outputFile;
	logDebug() << "Quiet:" << (args.quiet ? "true" : "false");

	Graph g;
	if (args.randomGraphSize) {
		g = std::move(Graph::randomGraph(args.randomGraphSize));
		if (!args.randomGraphOutputFile.empty()) {
			string err = g.toFile(args.randomGraphOutputFile);
			if (!err.empty()) {
				cerr << err << endl;
				return 1;
			}
			return 0;
		}
	} else {
		auto r = Graph::fromFile(args.inputFile);
		if (!r) {
			logError() << r.error();
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
