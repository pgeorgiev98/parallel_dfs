#include "arguments.h"
#include "graph.h"
#include "log.h"
#include "chronometer.h"

#include <iostream>
using namespace std;

int main(int argc, char **argv)
{
	Arguments args = Arguments::parseArguments(argc, argv);

	if (args.quiet)
		Logger::setMaximumLogLevel(Logger::LogLevel::Info);

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

	Chronometer c;
	logDebug() << "Traversing";
	c.start();
	g.traverseSingleThreaded();
	logInfo() << "Operation took" << c.milliseconds() << "ms";


	return 0;
}
