/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   graph.h
 * Author: mateus
 *
 * Created on March 13, 2018, 10:36 AM
 */

#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <deque>
//#include <tuple>
#include <set>
#include <climits> // this is for the maximum size of an integer (maximum size of a weight)



//#include <tuple>
using namespace std;

typedef long int Weight;
extern Weight MAX_WEIGHT; //global variable that defines the maximum value of a weight. This should be undertood as infinity.
// this variable is initialized in file graph.cpp
typedef int Vertex;
typedef std::vector<bool> SubsetTerminals;

bool isIncludedSubsetTerminals(SubsetTerminals S1, SubsetTerminals S2);

void dijkstra(const vector<map<Vertex,Weight> > &adjList, vector<Weight> &min_distance, vector<Vertex> &origin, Vertex source);
Vertex dijkstra(const vector<map<Vertex,Weight> > &adjList, vector<Weight> &min_distance, vector<Vertex> &origin, Vertex source, const vector<int> &terminalsMap, int T);
void dijkstra(const vector<map<Vertex,Weight> > &adjList, vector<Weight> &min_distance, vector<Vertex> &origin, Vertex source, Vertex v1, Vertex v2);
void dijkstra(const vector<map<Vertex,Weight> > &adjList, vector<Weight> &min_distance, vector<Vertex> &origin, Vertex source, Vertex target);
Vertex dijkstra(const vector<map<Vertex,Weight> > &adjList, vector<Weight> &min_distance, vector<Vertex> &origin,
        vector<bool>& inTree ,set< pair<Weight,Vertex> >& active_vertices);
void dijkstra(const vector<map<Vertex,Weight> > &adjList, vector<Weight> &min_distance, vector<Vertex> &origin,
        set < pair<Weight, Vertex> >& active_vertices);

class Graph{
    public:
        // A graph is represented by its adjacency list.
        // a weighted edge (i,j,w) belongs to the graph if the pair (j,w) belongs to adjList[i].
        vector<vector<Weight> > distanceMatrix;
        int numberVertices;
        int numberEdges;
        int numberTerminals;
        vector<std::map<Vertex,Weight> > adjList;
        // the vector terminals has as many positions as terminals. position i is the i-th terminal
        vector<int> terminals;
        // the vector terminalMap has as many positions as vertices in the graph. Position i is set to -1 if vertex i
        // is not a terminal, and is set to j if terminals[j] = i.
        vector<int> terminalsMap;

        vector<vector <long int> > DistTerminals; // computes the distance between terminals
        vector<vector <long int> > DistTerminalsVertices; // numberTerminals x numberVertices Table with distances between each terminal and each vertex
        Graph();
        Graph(istream& input);
        Graph(const char* filename);
        Weight edgeWeight(Vertex v1, Vertex v2);
        Weight distance(Vertex source, Vertex target) const;
        vector<Vertex> shortestPath(Vertex source, Vertex target);
        void printDistTerminalsVertices();
        void build_distTerminals();
        void build_distTerminalsVertices();
        void constructDistanceMatrix();
        void printDistanceMatrix();
        void print();
        void readGraph(istream& input);
        void addEdge(Vertex v1,Vertex v2, Weight w);
        void removeEdge(Vertex v1,Vertex v2); // Obs: we are considering simple graphs.
        bool find_next(std::istream& input, const std::string& pat, std::string& line);
};


void printSubsetTerminals(Graph &G, SubsetTerminals S);







#endif /* GRAPH_H */

