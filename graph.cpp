#include "graph.h"
#include "log.h"
#include "chronometer.h"

#include <fstream>
#include <random>
#include <stack>
#include <atomic>
#include <omp.h>

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
					if (visitedCount == nodeCount)
						return;
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

void Graph::traverse(int threads)
{
	omp_set_num_threads(threads);

	vector<atomic_flag> atomics(nodeCount);
	vector<bool> visited(nodeCount, false);
	atomic<int> visitedCount = 0;
	atomic<bool> allNodesTraversed = false;
#pragma omp parallel
	{
		int startingNode = 0;
		for (;;) {
BEGIN:
			if (allNodesTraversed.load())
				break;
			bool found = false;
			while (startingNode < nodeCount) {
				if (!visited[startingNode] && !atomics[startingNode].test_and_set()) {
					found = true;
					break;
				}
				++startingNode;
			}
			if (!found)
				break;

			visited[startingNode] = true;
			if (++visitedCount == nodeCount) {
				allNodesTraversed.store(true);
				goto BEGIN;
			}

			stack<int> st;
			st.push(startingNode);
			while (!st.empty()) {
				int node = st.top();
				int nextNode = -1;
				for (int n : relations[node]) {
					if (!visited[n] && !atomics[n].test_and_set()) {
						visited[n] = true;
						nextNode = n;
						if (++visitedCount == nodeCount) {
							allNodesTraversed.store(true);
							goto BEGIN;
						}
						break;
					}
				}
				if (nextNode == -1) {
					st.pop();
					if (allNodesTraversed.load())
						goto BEGIN;
				} else {
					st.push(nextNode);
				}
			}
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
	vector<char> v(nodeCount);
	for (int i = 0; i < nodeCount; ++i) {
		for (int j = 0; j < nodeCount; ++j)
			v[j] = '0';
		for (auto n : relations[i])
			v[n] = '1';
		out.write(v.data(), nodeCount);
		out.put('\n');
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

		long long position = in.tellg();
		in.seekg(0, in.end);
		long long remaining = (long long)in.tellg() - position;
		in.seekg(position, in.beg);

		vector<char> data(remaining);
		in.read(data.data(), remaining);

		if (!in)
			throw string("Failed to read from file");

		g.nodeCount = n;
		g.relations.resize(n);
		long long index = 0;
		for (int i = 0; i < n; ++i) {
			auto &nodeRel = g.relations[i];
			for (int j = 0; j < n; ++j) {
				while (index < data.size() && data[index] <= ' ')
					++index;
				if (index == data.size())
					throw "Unexpected EOF";
				char c = data[index++];
				if (c != '0' && c != '1')
					throw "Expected 1 or 0 at row " + to_string(i + 1) +
						", column " + to_string(j + 1);
				if (c == '1')
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
	Chronometer chr;
	chr.start();
	logInfo() << "Generating random graph with" << nodeCount << "nodes";

	Graph g;
	g.nodeCount = nodeCount;
	g.relations.resize(nodeCount);

	atomic<int> generatedNodesCount = 0;
#pragma omp parallel
	{
		random_device rd;
		mt19937 gen(rd());
		bernoulli_distribution dist(0.1);

		int thread = omp_get_thread_num();
		int threads = omp_get_num_threads();
		int block = nodeCount / threads;
		int begin = (thread * nodeCount) / threads;
		int end = ((thread + 1) * nodeCount) / threads;

		for (int i = begin; i < end; ++i) {
			auto &nodeRel = g.relations[i];
			for (int j = 0; j < nodeCount; ++j)
				if (dist(gen))
					nodeRel.push_back(j);

			int nc = ++generatedNodesCount;
			if (nc % 5000 == 0)
				logDebug().noSpace() << int(100.0 * double(nc) / nodeCount) << "\% Done [" << chr.milliseconds() << " ms]";
		}
	}

	logInfo() << "Graph generated for" << chr.milliseconds() << "ms";
	return g;
}
