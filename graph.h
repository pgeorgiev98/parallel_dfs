#ifndef GRAPH_H_INCLUDED
#define GRAPH_H_INCLUDED

#include "result.h"

#include <vector>
#include <string>

struct Graph
{
	std::vector<std::vector<int>> relations;
	int nodeCount;

	std::vector<int> traverseSingleThreaded();
	std::vector<std::vector<int>> traverse(int threads);
	std::string toFile(const std::string &path);

	static Result<Graph, std::string> fromFile(const std::string &path);
	static Graph randomGraph(int nodeCount, int maxEdgeCount);
};

#endif
