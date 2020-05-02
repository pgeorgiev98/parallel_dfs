#include "graph.h"
#include "log.h"

#include <fstream>
#include <random>
#include <stack>

using namespace std;

void Graph::traverseSingleThreaded()
{
	vector<bool> visited(nodeCount, false);
	int visitedCount = 0;
	int startingNode = 0;
	while (visitedCount < nodeCount) {
		stack<int> st;
		while (visited[startingNode])
			++startingNode;
		st.push(startingNode);
		visited[startingNode] = true;
		++visitedCount;
		while (!st.empty()) {
			int node = st.top();
			int nextNode = -1;
			for (int n : relations[node]) {
				if (!visited[n]) {
					visited[n] = true;
					++visitedCount;
					nextNode = n;
					break;
				}
			}
			if (nextNode == -1)
				st.pop();
			else
				st.push(nextNode);
		}
	}
}

string Graph::toFile(const string &path)
{
	logDebug() << "Saving graph to" << path;
	ofstream out(path, ofstream::out);
	if (!out) {
		// TODO: failure reason
		return "Failed to open file " + path;
	}

	out << nodeCount << endl;
	vector<bool> v(nodeCount);
	for (int i = 0; i < nodeCount; ++i) {
		for (int j = 0; j < nodeCount; ++j)
			v[j] = false;
		for (auto n : relations[i])
			v[n] = true;
		for (int j = 0; j < nodeCount; ++j)
			out << int(v[j]) << (j == nodeCount - 1 ? "" : " ");
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
			auto &nodeRel = g.relations[i];
			for (int j = 0; j < n; ++j) {
				char v;
				in >> v;
				if (!in || (v != '0' && v != '1'))
					throw "Expected 1 or 0 at row " + to_string(i + 1) +
						", column " + to_string(j + 1);
				if (v == '1')
					nodeRel.push_back(j);
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
		auto &nodeRel = g.relations[i];
		for (int j = 0; j < nodeCount; ++j)
			if (dist(gen))
				nodeRel.push_back(j);
	}

	logDebug() << "Graph generated";
	return g;
}
