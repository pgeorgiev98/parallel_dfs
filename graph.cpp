#include "graph.h"

#include <fstream>
#include <random>

using namespace std;


Result<Graph, std::string> Graph::fromFile(const std::string &path)
{
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
				int v;
				in >> v;
				if (!in || (v != 0 && v != 1))
					throw "Expected 1 or 0 at row " + to_string(i + 1) +
						", column " + to_string(j + 1);
				row[j] = v;
			}
		}

	} catch (string err) {
		return "Failed to parse file " + path + ": " + string(err);
	}

	return g;
}

Graph Graph::randomGraph(int nodeCount)
{
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

	return g;
}
