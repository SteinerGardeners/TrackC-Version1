#ifndef TREE_H
#define TREE_H

#include <memory>
#include <stack>

#include "graph.h"
#include "listgraph.h"

using namespace std;

class ListGraph;

class treeNode{
    public:
        Vertex v;
        Weight w; // weight of edge (v,Parent->v); // Obs trows an error if such an edge does not exist in the graph
        treeNode* parent; // NULL if root
        map<Vertex,shared_ptr<treeNode> > children;
        treeNode(Vertex v):v{v},w{MAX_WEIGHT}, parent{NULL}{}; //Build a root
        treeNode(Vertex v, Weight w, treeNode* parent):v{v},w{w},parent{parent}{};
        shared_ptr<treeNode> addChild(Vertex v, Weight w);
        shared_ptr<treeNode> addChild(shared_ptr<treeNode> N);
        void print();
        void printSubTree();
        int pruneLeaves(const vector<int>& terminalsMap);
};

class Tree{
    public:
        const Graph &G; // Pointer to a graph
        const vector<int> &terminalsMap;
        const vector<Vertex> &terminals;
        shared_ptr<treeNode> root; // the root of the tree
        Tree(const Graph &G, Vertex v):G{G}, terminalsMap{G.terminalsMap}, terminals{G.terminals},root{new treeNode(v)}{};
        Tree(const Graph &G):G{G}, terminalsMap{G.terminalsMap}, terminals{G.terminals}{};
        Tree(const Graph &G, treeNode N):G{G}, terminalsMap{G.terminalsMap}, terminals{G.terminals}, root{new treeNode(N)}{};
        Tree(const Graph &G, Vertex v, vector<int> &terminalsMap, vector<Vertex> &terminals):G{G}, terminalsMap{terminalsMap}, terminals{terminals},root{new treeNode(v)}{};
        Tree(const Graph &G, vector<int> &terminalsMap, vector<Vertex> &terminals):G{G}, terminalsMap{terminalsMap}, terminals{terminals}{};
        Tree(const Graph &G, treeNode N, vector<int> &terminalsMap, vector<Vertex> &terminals):G{G}, terminalsMap{terminalsMap}, terminals{terminals}, root{new treeNode(N)}{};
        void print();// Recursively prints the tree
        bool isTree(); // Returns true if each vertex of the graph occurs at most once.
        bool isConsistent();
        ListGraph convertToGraph(); // Transforms into a graph. // Obs: does this even if isTree() is false
        Tree subTree(treeNode N); // returns the subtree rooted at node N
        void replaceNode(treeNode N, Tree &T); // The subtree rooted at N is replaced with the subtree rooted at T
        void pruneRoot(); // prune the root if it's not a terminal node
        Weight pruneLeaves();// prune useless leaves en return the size of the tree
        int size();
};

#endif /* TREE_H */
