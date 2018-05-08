#ifndef LISTGRAPH_H
#define LISTGRAPH_H

#include <assert.h>
#include <memory>

#include "edge.h"
#include "tree.h"


using namespace std;

class Tree;

class ListGraph{ // represents graphs as a set of edges
    private:
        set<edge> S;
        const Graph &G;
    public:
        void print();
        ListGraph(const Graph &G):G{G}{};
        bool insertEdge(edge e);
        Tree spanningTree(Vertex v); // returns a spanning tree rooted at v. Trows error if v is not a vertex
        bool isConnected(); // returns true if connected
        ListGraph Union(ListGraph L) const; // returns union of both graphs.
        // Returns error if some edge occurs in both graphs with distinct weights.
};;

#endif /* LISTGRAPH_H */
