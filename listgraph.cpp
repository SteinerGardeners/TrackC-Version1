#include "listgraph.h"

using namespace std;

void ListGraph::print(){
        for (set<edge>::const_iterator it=S.begin(); it != S.end(); ++it){
            it->print();
        }
}

bool ListGraph::insertEdge(edge e){
    return S.insert(e).second && S.insert(e.reverse()).second;
}

Tree ListGraph::spanningTree(Vertex v){
    if (v < 0 || v > G.numberVertices) throw "ListGraph spanningTree " + to_string(v) + " is not a vertex";
    Tree T(G, v);
    set<pair<Weight, edge> > next;
    map<Vertex, shared_ptr<treeNode> > nodes;
    nodes.insert(pair<Vertex, shared_ptr<treeNode> >(v, T.root));
    for (map<Vertex, Weight>::const_iterator it = G.adjList[v].begin(); it != G.adjList[v].end(); ++it){
        if (S.find(edge(v, it->first)) != S.end()) next.insert(pair<Weight, edge>(it->second,edge(v, it->first)));
    }
    while (!next.empty()){
        edge current = next.begin()->second;
        Weight w = next.begin()->first;
        next.erase(next.begin());
        if (nodes.find(current.v2) == nodes.end()){
            shared_ptr<treeNode> parent = nodes.find(current.v1)->second;
            shared_ptr<treeNode> N = parent->addChild(current.v2,w);
            nodes.insert(pair<Vertex, shared_ptr<treeNode> >(current.v2, N));
            for (map<Vertex,Weight>::const_iterator it = G.adjList[current.v2].begin(); it != G.adjList[current.v2].end(); ++it){
                if(nodes.find(it->first) == nodes.end() && S.find(edge(current.v2, it->first)) != S.end())
                    next.insert(pair<Weight,edge>(it->second,edge(current.v2,it->first)));
            }
        }
    }
    return T;
}

bool ListGraph::isConnected(){
    stack<Vertex> next;
    set<Vertex> seen;
    next.push(S.begin()->v1);
    seen.insert(S.begin()->v1);
    while (!next.empty()){
        Vertex current = next.top();
        next.pop();
        for (map<Vertex,Weight>::const_iterator it = G.adjList[current].begin(); it != G.adjList[current].end(); ++it){
            if(S.find(edge(current, it->first)) != S.end() && seen.find(it->first) == seen.end()){
                seen.insert(it->first);
                next.push(it->first);
            }
        }
    }


    for (set<edge>::iterator it = S.begin(); it != S.end(); ++it){
        if (seen.find(it->v1) == seen.end() || seen.find(it->v2) == seen.end())
            return false;
    }
    return true;
}

ListGraph ListGraph::Union(ListGraph L) const {
    ListGraph new_L(G);
    new_L.S = S;
    new_L.S.insert(L.S.begin(), L.S.end());
    return new_L;
}
