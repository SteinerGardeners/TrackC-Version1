/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "graph.h"
#include <vector>
#include <map>
#include <iostream>
#include <algorithm>
#include <assert.h>
using namespace std;

Weight MAX_WEIGHT = INT_MAX;  // Initialization of the global variable with maximum weight


Graph::Graph(){} //empty constructor

Graph::Graph(istream& input){
    readGraph(input);
}

Graph::Graph(const char* filename){
    ifstream input(filename);
    readGraph(input);
    input.close();
}

void Graph::printDistTerminalsVertices(){
	for (int i = 0; i< numberTerminals; i++){
		for (int j=0; j< numberVertices; j++){
			cout << "Distance between " << terminals[i] + 1 << " and  " << j+1 << " : " << DistTerminalsVertices[i][j] << endl; 
		}
	}
}

bool Graph::find_next(std::istream& input, const std::string& pat, std::string& line){
    while(getline(input, line)){
        if (line == pat){return true;}
    }
    return false;
}


void Graph::constructDistanceMatrix(){
    // initializes Distance Matrix
    vector<Weight> maxDistRow(numberVertices,MAX_WEIGHT);
    for(int i=0; i<numberVertices ; i++){
        distanceMatrix.push_back(maxDistRow);
    }
    for (int i=0; i<numberVertices; i++){
        for(std::map<Vertex,Weight>::iterator it = adjList[i].begin(); it !=adjList[i].end(); it++){
            distanceMatrix[i][(*it).first]  = (*it).second;
        }
    }
    for(int i=0; i<numberVertices; i++){
        distanceMatrix[i][i]= 0;
    }
    for(int k = 0; k< numberVertices; k++){
        for(int i = 0; i< numberVertices; i++){
            for(int j = 0; j< numberVertices; j++){
                if (distanceMatrix[i][j] > distanceMatrix[i][k]+ distanceMatrix[k][j]){
                    distanceMatrix[i][j] = distanceMatrix[i][k] + distanceMatrix[k][j];
                }
            }
        }
    }
}

void Graph::printDistanceMatrix(){
    cout << endl << "PRINTING DISTANCE MATRIX: " << endl << endl;
    for (int i = 0 ; i< numberVertices ; i++){
        for (int j= 0 ; j< numberVertices; j++){
            cout << "(" << i +1  << "," << j +1 << "," << distanceMatrix[i][j] << ")" << endl;
        }
    }
}

Weight Graph::edgeWeight(Vertex v1, Vertex v2){
    std::map<Vertex,Weight>::iterator it;
    it = adjList[v1].find(v2);
    assert ( it != adjList[v1].end() );
    return it->second;
}

struct terminals_condition{
    const vector<int> * is_terminal;
    int T;
    int count;
};


void dijkstra(const vector<map<Vertex,Weight> > &adjList, vector<Weight> &min_distance, vector<Vertex> &origin, Vertex source){
    min_distance[ source ] = 0;
    set< pair<Weight,Vertex> > active_vertices;
    active_vertices.insert( {0,source} );

    while (!active_vertices.empty()) {
        Vertex where = active_vertices.begin()->second;
        active_vertices.erase( active_vertices.begin() );
        for (std::map<Vertex,Weight>::const_iterator it=adjList[where].begin(); it!=adjList[where].end(); ++it)
            if (min_distance[it->first] > min_distance[where] + it->second) {
                active_vertices.erase( { min_distance[it->first], it->first } );
                min_distance[it->first] = min_distance[where] + it->second;
                origin[it->first] = where;
                active_vertices.insert( { min_distance[it->first], it->first } );
            }
    }
}

void dijkstra(const vector<map<Vertex,Weight> > &adjList, vector<Weight> &min_distance, vector<Vertex> &origin,
        set < pair<Weight, Vertex> >& active_vertices){

    while (!active_vertices.empty()) {
        Vertex where = active_vertices.begin()->second;
        active_vertices.erase( active_vertices.begin() );
        for (std::map<Vertex,Weight>::const_iterator it=adjList[where].begin(); it!=adjList[where].end(); ++it)
            if (min_distance[it->first] > min_distance[where] + it->second) {
                active_vertices.erase( { min_distance[it->first], it->first } );
                min_distance[it->first] = min_distance[where] + it->second;
                origin[it->first] = where;
                active_vertices.insert( { min_distance[it->first], it->first } );
            }
    }
}

Vertex dijkstra(const vector<map<Vertex,Weight> > &adjList, vector<Weight> &min_distance, vector<Vertex> &origin, Vertex source, const vector<int> &terminalsMap, int T){
    min_distance[ source ] = 0;
    set< pair<Weight,Vertex> > active_vertices;
    active_vertices.insert( {0,source} );

    while (!active_vertices.empty()) {
        Vertex where = active_vertices.begin()->second;
        if ( terminalsMap[where] != -1) --T;
        if (!T) return where;
        active_vertices.erase( active_vertices.begin() );
        for (std::map<Vertex,Weight>::const_iterator it=adjList[where].begin(); it!=adjList[where].end(); ++it)
            if (min_distance[it->first] > min_distance[where] + it->second) {
                active_vertices.erase( { min_distance[it->first], it->first } );
                min_distance[it->first] = min_distance[where] + it->second;
                origin[it->first] = where;
                active_vertices.insert( { min_distance[it->first], it->first } );
            }
    }
    return -1;
}

Vertex dijkstra(const vector<map<Vertex, Weight> > &adjList, vector<Weight> &min_distance, vector<Vertex> &origin,
        vector<bool>& inTree, set< pair<Weight, Vertex> >& active_vertices){

    while (!active_vertices.empty()) {
        Vertex where = active_vertices.begin()->second;
        if (inTree[where]) return where;
        active_vertices.erase( active_vertices.begin() );
        for (std::map<Vertex,Weight>::const_iterator it=adjList[where].begin(); it!=adjList[where].end(); ++it)
            if (min_distance[it->first] > min_distance[where] + it->second) {
                active_vertices.erase( { min_distance[it->first], it->first } );
                min_distance[it->first] = min_distance[where] + it->second;
                origin[it->first] = where;
                active_vertices.insert( { min_distance[it->first], it->first } );
            }
    }
    return -1;
}

void dijkstra(const vector<map<Vertex,Weight> > &adjList, vector<Weight> &min_distance, vector<Vertex> &origin, Vertex source, Vertex target){
    min_distance[ source ] = 0;
    set< pair<Weight,Vertex> > active_vertices;
    active_vertices.insert( {0,source} );

    while (!active_vertices.empty()) {
        Vertex where = active_vertices.begin()->second;
        if (where == target) return;
        active_vertices.erase( active_vertices.begin() );
        for (std::map<Vertex,Weight>::const_iterator it=adjList[where].begin(); it!=adjList[where].end(); ++it)
            if (min_distance[it->first] > min_distance[where] + it->second) {
                active_vertices.erase( { min_distance[it->first], it->first } );
                min_distance[it->first] = min_distance[where] + it->second;
                origin[it->first] = where;
                active_vertices.insert( { min_distance[it->first], it->first } );
            }
    }
}

void dijkstra(const vector<map<Vertex,Weight> > &adjList, vector<Weight> &min_distance, vector<Vertex> &origin, Vertex source, Vertex v1, Vertex v2){
    min_distance[ source ] = 0;
    set< pair<Weight,Vertex> > active_vertices;
    active_vertices.insert( {0,source} );

    while (!active_vertices.empty()) {
        Vertex where = active_vertices.begin()->second;
        if (where == v1 || where == v2) return;
        active_vertices.erase( active_vertices.begin() );
        for (std::map<Vertex,Weight>::const_iterator it=adjList[where].begin(); it!=adjList[where].end(); ++it)
            if (min_distance[it->first] > min_distance[where] + it->second) {
                active_vertices.erase( { min_distance[it->first], it->first } );
                min_distance[it->first] = min_distance[where] + it->second;
                origin[it->first] = where;
                active_vertices.insert( { min_distance[it->first], it->first } );
            }
    }
}

Weight Graph::distance(Vertex source, Vertex target) const{
    vector<Weight> min_distance( numberVertices, MAX_WEIGHT );
    vector<Vertex> origin( adjList.size(), -1 );

    dijkstra(adjList, min_distance, origin, source, target);

    return min_distance[target];
}

vector<Vertex> Graph::shortestPath(Vertex source, Vertex target){
    vector<Weight> min_distance( adjList.size(), MAX_WEIGHT );
    vector<Vertex> origin( adjList.size(), -1 );
    vector<Vertex> path;
    Vertex current = target;

    dijkstra(adjList, min_distance, origin, source, target);

    while (current != -1){
        path.push_back(current);
        current = origin[current];
    }
    reverse(path.begin(), path.end());
    return path;
}

void Graph::build_distTerminalsVertices(){
    // The table distVerticesTerminals has numberTerminals rows and numberVertices  columns
    DistTerminalsVertices.reserve(numberTerminals);
    for ( int i = 0; i < numberTerminals; ++i){
        vector<Weight> min_distance( adjList.size(), MAX_WEIGHT );
        vector<Vertex> origin( adjList.size(), -1 );
        dijkstra(adjList, min_distance, origin, terminals[i]);
        DistTerminalsVertices.push_back(min_distance);
    }
}

void Graph::addEdge(Vertex v1, Vertex v2, Weight w){
    adjList[v1].insert(std::pair<Vertex,Weight>(v2,w));
    adjList[v2].insert(std::pair<Vertex,Weight>(v1,w));
    // Obs: If we are solving the Steiner Tree problem on undirected graphs,
    // then both directions of the edge must be added, otherwise the algorithm
    // will not work properly.
    // If we are interested in solving the Steiner tree problem on DIRECTED graphs,
    // then only the first line should be present.
    // This is due to the fact that in the function SteinerSolver::constructTable,
    // the table is updated one time for each direction of edges.
}

void Graph::print(){
    cout << endl;
    cout << "printing Graph" << endl;
    cout << endl;
    cout << "Number of Vertices: " <<  this->numberVertices << endl;
    cout << "Number of Edges: " << this->numberEdges << endl;
    cout << "Number of Terminals: " << this->numberTerminals << endl;
    cout << endl;
    cout << "Terminals: " << endl;
    // print Terminals
    for(int i=0; i< numberTerminals; i++){
        cout << this->terminals[i] + 1 << " " ;  // adds +1 back to the vertex number
    }
    cout << endl << endl;
    // print Edges
    for(Vertex v1 = 0; v1< this->numberVertices ; v1++){
        //cout << "printing Vertex Number: " << v1 + 1 << endl;
        for(std::map<Vertex,Weight>::iterator it= adjList[v1].begin(); it != adjList[v1].end(); it++){
            // Obs: When printing, adds 1 back to the vertex number.
            cout << "(" << v1 +1 << "," << (*it).first + 1 << "," << (*it).second << ")"<< endl;
        }
    }

}

void Graph::readGraph(istream& input){
    std::string line;

    // Find graph section
    find_next(input, "SECTION Graph", line);

    // Get number of vertices
    getline(input, line);
    std::stringstream(line).ignore(256, ' ') >> (this->numberVertices);
    adjList.reserve(this->numberVertices);
    for (int i = 0; i< this->numberVertices; i++){
        std::map<Vertex,Weight> tmpmap; // creates an empty map.
        adjList.push_back(tmpmap);
    }

    // Get number of edges
    getline(input, line);
    std::stringstream(line).ignore(256, ' ') >> (this->numberEdges);

    // Read edges
    for (int i=0; i< (this->numberEdges); ++i){
        Vertex v1, v2;
        Weight w;
        getline(input, line);
        std::stringstream(line).ignore(256, ' ') >> v1 >> v2 >> w;
        //cout << "testEdge: (" << v1 << "," << v2 << "," << w << ")" << endl;
        v1--; v2--;
        addEdge(v1,v2,w);
    }

    // Get terminals
    find_next(input, "SECTION Terminals", line);

    // Get the number of terminals
    getline(input, line);
    std::stringstream(line).ignore(256, ' ') >> (this->numberTerminals);

    // initialize Terminals
    terminals.resize(this->numberTerminals,0);
    terminalsMap.resize(this->numberVertices,-1); // all entries in terminalsMap are initialized to -1
    for (int i= 0; i < this->numberTerminals; ++i){
        getline(input, line);
        std::stringstream(line).ignore(256, ' ') >> terminals[i];
        terminals[i]--; // subtracts 1 because vertices are numbered from 0
        terminalsMap[terminals[i]] = i;
    }
}


void printSubsetTerminals(Graph &G, SubsetTerminals S){
    cout << "{ " ;
    for (unsigned int i=0; i< S.size(); i++){
        if (S[i]==true){
            cout << G.terminals[i] + 1; // adds 1 back to the name of a vertex
            cout << " " ;
        }
    }
    cout << "}";
}


