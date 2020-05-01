#ifndef GRAPH_H_INCLUDED
#define GRAPH_H_INCLUDED

#include "result.h"

#include <vector>
#include <string>

struct Graph
{
	std::vector<std::vector<bool>> relations;
	int nodeCount;

	std::string toFile(const std::string &path);

	static Result<Graph, std::string> fromFile(const std::string &path);
	static Graph randomGraph(int nodeCount);
};

#endif
