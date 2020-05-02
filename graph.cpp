#include "graph.h"
#include "log.h"

#include <fstream>
#include <random>

using namespace std;

string Graph::toFile(const string &path)
{
	logDebug() << "Saving graph to" << path;
	ofstream out(path, ofstream::out);
	if (!out) {
		// TODO: failure reason
		return "Failed to open file " + path;
	}

	out << nodeCount << endl;
	for (int i = 0; i < nodeCount; ++i) {
		for (int j = 0; j < nodeCount; ++j) {
			out << int(relations[i][j]) << (j == nodeCount - 1 ? "" : " ");
		}
		out << endl;
	}

	if (!out) {
		// TODO: failure reason
		return "Failed to write to " + path;
	}

	logDebug() << "Graph saved";
	return string();
}

Result<Graph, std::string> Graph::fromFile(const std::string &path)
{
	logDebug() << "Loading graph from file" << path;
	ifstream in(path, ifstream::in);
	if (!in) {
		// TODO: failure reason
		return "Failed to open file " + path;
	}

	Graph g;
	int n;
	try {
		in >> n;
		if (!in)
			throw string("Expected number of nodes");

		g.nodeCount = n;
		g.relations.resize(n);
		for (int i = 0; i < n; ++i) {
			auto &row = g.relations[i];
			row.resize(n);
			for (int j = 0; j < n; ++j) {
				char v;
				in >> v;
				if (!in || (v != '0' && v != '1'))
					throw "Expected 1 or 0 at row " + to_string(i + 1) +
						", column " + to_string(j + 1);
				row[j] = v - '0';
			}
		}

	} catch (string err) {
		return "Failed to parse file " + path + ": " + string(err);
	}

	logDebug() << "Graph loaded";
	return g;
}

Graph Graph::randomGraph(int nodeCount)
{
	logDebug() << "Generating random graph with" << nodeCount << "nodes";
	random_device rd;
	mt19937 gen(rd());
	bernoulli_distribution dist(0.5);

	Graph g;
	g.nodeCount = nodeCount;
	g.relations.resize(nodeCount);

	for (int i = 0; i < nodeCount; ++i) {
		auto &row = g.relations[i];
		row.resize(nodeCount);
		for (int j = 0; j < nodeCount; ++j)
			row[j] = dist(gen);
	}

	logDebug() << "Graph generated";
	return g;
}
