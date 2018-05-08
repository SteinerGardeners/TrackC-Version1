#include "init.h"

Weight weightInitialTree(Graph& G, Vertex root, vector<Vertex> &terminalsMap, vector<Vertex> &terminals){
	return 	incrementalDijks3(G,root,terminalsMap,terminals).pruneLeaves(); 
}


Tree spanningTree(Graph& G, Vertex root){
    if (root < 0 || root > G.numberVertices) throw "spanningTree";
    Tree T(G,root);
    set<pair<Weight,edge> > next;
    map<Vertex, shared_ptr<treeNode> > nodes;
    nodes.insert(pair<Vertex,shared_ptr<treeNode> >(root, T.root));
    for (map<Vertex, Weight>::const_iterator it = G.adjList[root].begin(); it != G.adjList[root].end(); ++it){
        next.insert(pair<Weight, edge>(it->second,edge(root, it->first)));
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
                if(nodes.find(it->first) == nodes.end())
                    next.insert(pair<Weight,edge>(it->second,edge(current.v2,it->first)));
            }
        }
    }
    return T;
}

inline void steiner_3(Graph &G, vector<Weight> &min_distance0, vector<Vertex> &origin0, Vertex v0, Vertex v1, Vertex v2){
    vector<Weight> min_distance1( G.numberVertices, MAX_WEIGHT );
    vector<Vertex> origin1( G.numberVertices, -1 );
    vector<Weight> min_distance2( G.numberVertices, MAX_WEIGHT );
    vector<Vertex> origin2( G.numberVertices, -1 );

    dijkstra(G.adjList, min_distance1, origin1, v1, v0, v2);
    if (min_distance0[v2] == min_distance1[v2]){
        dijkstra(G.adjList, min_distance2, origin2, v2, v0, v1);
    }else{
        if (min_distance0[v2] < min_distance1[v2]){
            dijkstra(G.adjList, min_distance2, origin2, v2, v0);
        }else{
            dijkstra(G.adjList, min_distance2, origin2, v2, v1);
        }
    }
    Vertex intersect=-1;
    Weight distIntersect = MAX_WEIGHT;
    for (Vertex i = 0; i < G.numberVertices; ++i){
        if (min_distance2[i] != MAX_WEIGHT && min_distance1[i] != MAX_WEIGHT && min_distance0[i] != MAX_WEIGHT &&
                min_distance0[i] + min_distance1[i] + min_distance2[i] < distIntersect){
            intersect = i;
            distIntersect = min_distance0[i] + min_distance1[i] + min_distance2[i];
        }
    }
    //Merge origin1 into origin0
    Vertex current = intersect;
    while (origin1[current] != -1){
        if (origin1[current] != origin0[current]){
            origin0[origin1[current]] = current;
        }
        current = origin1[current];
    }
    //Merge origin2 into origin0
    current = intersect;
    while (origin2[current] != -1){
        if (origin2[current] != origin0[current]){
            origin0[origin2[current]] = current;
        }
        current = origin2[current];
    }

}

Tree incrementalDijks3(Graph& G, Vertex root, vector<int> &terminalsMap, vector<Vertex> &terminals){
    vector<std::map<Vertex,Weight> > backup_adjList = G.adjList;
    vector<Weight> min_distance( G.numberVertices, MAX_WEIGHT );
    vector<Vertex> origin( G.numberVertices, -1 );
    Vertex far = dijkstra(G.adjList, min_distance, origin, root, terminalsMap, G.numberTerminals);
    Weight far_dist = min_distance[far];

    Vertex last_far = far;
    bool dijk_simple = true;
    while (far_dist > 0){
        if (dijk_simple){
            dijk_simple = false;
            vector<Weight> backup_distance = min_distance;
            vector<Vertex> backup_origin = origin;

            // Update graph, set edge to 0
            last_far = far;
            Vertex current=far;
            while (origin[current] != -1 && backup_distance[current] != 0){
                backup_distance[current] = 0;
                current = origin[current];
            }
            current=far;
            set< pair<Weight,Vertex> > active_vertices;
            while (origin[current] != -1 && min_distance[current] != 0){
                for (map<Vertex,Weight>::iterator it = G.adjList[current].begin(); it != G.adjList[current].end(); ++it){
                    if (it->first != backup_origin[current] && backup_distance[it->first] > it->second){
                        active_vertices.erase({backup_distance[it->first], it->first});
                        active_vertices.insert({it->second, it->first});
                        backup_distance[it->first] = it->second;
                        backup_origin[it->first] = current;
                    }
                }
                current = origin[current];
            }
            dijkstra(G.adjList, backup_distance, backup_origin, active_vertices);
            far_dist = 0;
            for (vector<Vertex>::iterator it = terminals.begin(); it != terminals.end(); ++it){
                if (far_dist < backup_distance[*it]){
                    far_dist = backup_distance[*it];
                    far = *it;
                }
            }
        }else{
            dijk_simple = true;
            steiner_3(G, min_distance, origin, root, far, last_far);

            // Update graph
            Vertex current=far;
            while (origin[current] != -1){
                G.adjList[current][origin[current]] = 0;
                G.adjList[origin[current]][current] = 0;
                min_distance[current] = MAX_WEIGHT;
                current = origin[current];
            }
            current= last_far;
            while (origin[current] != -1){
                G.adjList[current][origin[current]] = 0;
                G.adjList[origin[current]][current] = 0;
                min_distance[current] = MAX_WEIGHT;
                current = origin[current];
            }
            far = dijkstra(G.adjList, min_distance, origin, root, terminalsMap, G.numberTerminals);
            if (far == -1){
                far_dist = 0;
                for (vector<Vertex>::iterator it = terminals.begin(); it != terminals.end(); ++it){
                    if (far_dist < min_distance[*it]){
                        far_dist = min_distance[*it];
                        far = *it;
                    }
                }
            }else{
                far_dist = min_distance[far];
            }
        }
    }
    G.adjList = backup_adjList;

    //build tree
    Tree T(G, root, terminalsMap, terminals);
    vector<shared_ptr<treeNode> > nodes;
    for (int i  = 0; i < G.numberVertices; ++i){
        nodes.push_back(shared_ptr<treeNode>(new treeNode(i)));
    }
    nodes[root] = T.root;
    for (int i = 0; i < G.numberVertices; ++i){
        if (origin[i] != -1){
            nodes[i]->parent = nodes[origin[i]].get();
            nodes[i]->w = G.adjList[i][origin[i]];
            nodes[origin[i]]->addChild(nodes[i]);
        }
    }
    return T;
}
