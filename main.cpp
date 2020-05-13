#include "arguments.h"
#include "graph.h"
#include "log.h"
#include "chronometer.h"
#include "csv.h"

#include <iostream>
using namespace std;

int main(int argc, char **argv)
{
	Arguments args = Arguments::parseArguments(argc, argv);

	if (args.quiet)
		Logger::setMaximumLogLevel(Logger::LogLevel::Info);

	Graph g;
	if (args.randomGraphSize) {
		g = std::move(Graph::randomGraph(args.randomGraphSize, args.randomGraphMaxEdgeCount));
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

	CsvExporter csv;
	csv << "Command:" << args.commandLine << CsvExporter::endl;

	csv << "Threads\\Time (ms)";
	for (int i = 1; i <= args.passes; ++i)
		csv << ("Pass " + to_string(i));
	csv << CsvExporter::endl;

	Chronometer c;
	vector<vector<int>> output;
	for (int threads : args.threadCount) {
		csv << threads;
		for (int pass = 0; pass < args.passes; ++pass) {
			logInfo().noNewLine() << "Traversing pass " + to_string(pass + 1) + "/" + to_string(args.passes) << "with" << threads << "threads: ";
			c.start();
			if (threads == 1)
				output = {g.traverseSingleThreaded()};
			else
				output = g.traverse(threads);
			int ms = c.milliseconds();
			logInfo() << ms << "ms";
			csv << ms;
		}
		csv << CsvExporter::endl;
	}

	if (!args.outputCsvFile.empty())
		if (!csv.write(args.outputCsvFile))
			cerr << "Failed to save csv file" << endl;

	if (!args.outputFile.empty()) {
		ofstream of(args.outputFile, ofstream::out);
		if (!of) {
			logError() << "Failed to open output file"; // TODO: why?
		} else {
			for (const auto &o : output)
				for (int i = 0; i < o.size(); ++i)
					of << o[i] << (i == o.size() - 1 ? '\n' : ' ');
			if (!of)
				logError() << "An error occurred while writing to output file"; // TODO: what?
		}
	}

	return 0;
}
