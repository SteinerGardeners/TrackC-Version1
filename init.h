#pragma once

#include <stack>

#include "graph.h"
#include "tree.h"
#include "edge.h"

Tree spanningTree(Graph& G, Vertex root);
Tree incrementalDijks3(Graph& G, Vertex root, vector<int> &terminalsMap, vector<Vertex> &terminals);
// the fucntion below creates an initial tree and returns its weight
Weight weightInitialTree(Graph& G, Vertex root, vector<Vertex> &terminalsMap, vector<Vertex> &terminals);
