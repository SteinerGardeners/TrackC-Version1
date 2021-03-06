#include "opt.h"


volatile sig_atomic_t tle = false;

void signalHandler( int ) {
    tle = true;
}

void sonofroot(treeNode* node, Vertex root){
    treeNode* tmp = node;
    while (tmp != NULL){
        if (tmp->parent != NULL && tmp->parent->parent != NULL && tmp->v == tmp->parent->parent->v) throw 1;
        if (tmp->v == root) return;
        tmp = tmp->parent;
    }
    cout << "AAAAAAAAAAHHHHHHHHHHHHHH" << endl;
}

void printpathtoroot(treeNode* node){
    while (node != NULL){
        cout << node->v << " ";
        node = node->parent;
    }
    cout << endl;
}

void printpathtoorigin(Vertex v, vector<Vertex> &origin){
    while (v != -1){
        cout << v << " ";
        v = origin[v];
    }
    cout << endl;
}

bool cycle(treeNode* root){
    stack<treeNode*> next;
    set<Vertex> seen;
    next.push(root);
    seen.insert(root->v);
    while (!next.empty()){
        treeNode* current = next.top();
        next.pop();
        for (map<Vertex, shared_ptr<treeNode> >::iterator it = current->children.begin(); it != current->children.end(); ++it){
            if (!seen.insert(it->first).second) return false;
            next.push(it->second.get());
        }
    }
    return true;
}

bool cycleup(treeNode* node){
    treeNode* current = node;
    set<Vertex> seen;
    while (current != NULL){
        if (!seen.insert(current->v).second) return false;
        current = current->parent;
    }
    return true;
}

bool sonof(treeNode* node, Vertex v){
    stack<treeNode*> next;
    next.push(node);
    while(!next.empty()){
        treeNode* current = next.top();
        next.pop();
        if (current->v == v) return true;
        for (map<Vertex, shared_ptr<treeNode> >::iterator it = current->children.begin(); it != current->children.end(); ++it){
            next.push(it->second.get());
        }
    }
    return false;
}

inline void initPriorityQueue(const shared_ptr<treeNode> &node, const vector<map<Vertex, Weight> > &adjList,
        vector<Weight> &min_distance, vector<Vertex> &origin, set<pair<Weight, Vertex> > &active_vertices){
    queue<shared_ptr<treeNode> > next;
    next.push(node);
    while (!next.empty()){
        shared_ptr<treeNode> current = next.front();
        next.pop();

        for (map<Vertex,Weight>::const_iterator it = adjList[current->v].begin(); it != adjList[current->v].end(); ++it){
            map<Vertex, shared_ptr<treeNode> >::iterator son = current->children.find(it->first);
            if (son != current->children.end()){
                next.push(son->second);
                active_vertices.erase({min_distance[it->first], it->first});
                min_distance[it->first] = 0;
                origin[it->first] = current->v;
            }else if (min_distance[it->first] > it->second){
                active_vertices.erase({min_distance[it->first], it->first});
                active_vertices.insert( {it->second, it->first});
                min_distance[it->first] = it->second;
                origin[it->first] = current->v;
            }
        }
    }
}

inline treeNode* deleteUpPath(treeNode* tmp, const vector<int> &terminalsMap, vector<bool> &inTree){
    while (tmp->children.empty() && terminalsMap[tmp->v] == -1 && tmp->parent != NULL ){
        Vertex v = tmp->v;
        inTree[v] = false;
        tmp = tmp->parent;
        tmp->children.erase(v);
    }
    return tmp;
}

inline void buildDownPath(const vector<map<Vertex, Weight> > &adjList, const vector<Weight> &min_distance,
        const vector<Vertex> &origin, Vertex cible, vector<bool> &inTree, vector<weak_ptr<treeNode> > &mapNode){
    shared_ptr<treeNode> current = mapNode[cible].lock();
    while (origin[current->v] != -1){
        inTree[origin[current->v]] = true;
        if (mapNode[origin[current->v]].expired()){
            current = current->addChild(origin[current->v], min_distance[current->v] - min_distance[origin[current->v]]);
            mapNode[current->v] = current;
        }else{
            shared_ptr<treeNode> son = mapNode[origin[current->v]].lock();
            if (son->parent != NULL){
                son->parent->children.erase(son->v);
            }
            son->parent = current.get();
            son->w = adjList[son->v].find(current->v)->second;
            current = current->addChild(son);
        }
    }
}

inline void buildUpPath(shared_ptr<treeNode> current, const vector<map<Vertex, Weight> > &adjList,
        const vector<Weight> &min_distance, const vector<Vertex> &origin, vector<bool> &inTree,
        vector<weak_ptr<treeNode> > &mapNode){
    while (origin[current->v] != -1){
        inTree[current->v] = true;
        if (current->parent != NULL)
            current->parent->children.erase(current->v);
        if (mapNode[origin[current->v]].expired()){
            shared_ptr<treeNode> tmp(new treeNode(origin[current->v]));
            mapNode[tmp->v] = tmp;
            tmp->addChild(current);
            current->parent = tmp.get();
            current->w = min_distance[current->v] - min_distance[origin[current->v]];// vrai ?
            current = tmp;
        }else{
            shared_ptr<treeNode> tmp = mapNode[origin[current->v]].lock();
            tmp->addChild(current);
            //if (current->parent != NULL && current->parent->v == origin[current->v]) break;
            current->parent = tmp.get();
            current->w = adjList[current->v].find(tmp->v)->second;
            current = tmp;
        }
    }
}

inline void opt_d2(shared_ptr<treeNode> &node, const int numberVertices, const vector<map<Vertex, Weight> > &adjList,
        const vector<int> &terminalsMap, vector<weak_ptr<treeNode> > &mapNode , vector<bool> &inTree){
    if (node->parent == NULL || tle) return;
    node->parent->children.erase(node->v);

    //delete path to the first usefull parent
    deleteUpPath(node->parent, terminalsMap, inTree);

    node->parent = NULL;
    node->w=MAX_WEIGHT;

    set< pair<Weight,Vertex> > active_vertices;
    vector<Weight> min_distance( numberVertices, MAX_WEIGHT );
    vector<Vertex> origin( numberVertices, -1 );
    min_distance[node->v] = 0;

    // Add vertex to the priority queue
    initPriorityQueue(node, adjList, min_distance, origin, active_vertices);

    // Compute the shortest path for node to the reste of the tree
    Vertex cible = dijkstra(adjList, min_distance, origin, inTree, active_vertices);

    // Build the tree with origin
    buildDownPath(adjList, min_distance, origin, cible, inTree, mapNode);
}

inline void opt_d3(const shared_ptr<treeNode> &root, shared_ptr<treeNode> &v1, shared_ptr<treeNode> &v2,
        const int numberVertices, const vector<map<Vertex, Weight> > &adjList, vector<Vertex> terminalsMap,
        vector<weak_ptr<treeNode> > &mapNode, vector<bool> &inTree){


    if (v1->parent != NULL){
        v1->parent->children.erase(v1->v);
        deleteUpPath(v1->parent, terminalsMap, inTree);
    }
    if (v2->parent != NULL){
        v2->parent->children.erase(v2->v);
        deleteUpPath(v2->parent, terminalsMap, inTree);
    }

    while (v1->children.size() == 1 && terminalsMap[v1->v] == -1){
        inTree[v1->v] = false;
        v1 = v1->children.begin()->second;
    }
    v1->parent = NULL;
    v1->w = MAX_WEIGHT;

    while (v2->children.size() == 1 && terminalsMap[v2->v] == -1){
        inTree[v2->v] = false;
        v2 = v2->children.begin()->second;
    }
    v2->parent = NULL;
    v2->w = MAX_WEIGHT;

    // Dijkstra from v1
    set< pair<Weight,Vertex> > active_vertices;
    vector<Weight> min_distance1( numberVertices, MAX_WEIGHT );
    vector<Vertex> origin1( numberVertices, -1 );
    min_distance1[v1->v] = 0;

    initPriorityQueue(v1, adjList, min_distance1, origin1, active_vertices);
    Vertex c1 = dijkstra(adjList, min_distance1, origin1, inTree, active_vertices);

    // Dijkstra from v2
    active_vertices.clear();
    vector<Weight> min_distance2( numberVertices, MAX_WEIGHT );
    vector<Vertex> origin2( numberVertices, -1 );
    min_distance2[v2->v] = 0;

    initPriorityQueue(v2, adjList, min_distance2, origin2, active_vertices);
    Vertex c2 = dijkstra(adjList, min_distance2, origin2, inTree, active_vertices);

    // Dijkstra from root
    active_vertices.clear();
    vector<Weight> min_distance0( numberVertices, MAX_WEIGHT );
    vector<Vertex> origin0( numberVertices, -1 );
    min_distance0[root->v] = 0;

    initPriorityQueue(root, adjList, min_distance0, origin0, active_vertices);
    Vertex c0 = dijkstra(adjList, min_distance0, origin0, inTree, active_vertices);

    Vertex r1;
    if (min_distance2[c1] == 0){
        r1 = v2->v;
    }else{
        r1 = root->v;
    }
    Vertex r2;
    if (min_distance1[c2] == 0){
        r2 = v1->v;
    }else{
        r2 = root->v;
    }
    Vertex r0;
    if (min_distance1[c0] == 0){
        r0 = v1->v;
    }else{
        r0 = v2->v;
    }

    Vertex intersect=-1;
    Weight distIntersect = MAX_WEIGHT;
    if (r1 == root->v || r2 == root->v) {
        distIntersect = min_distance1[c1] + min_distance2[c2];
    }else if ( r0 == v1->v ) {
        distIntersect = min_distance0[c0] + min_distance2[c2];
    }else{
        distIntersect = min_distance0[c0] + min_distance1[c1];
    }
    assert( min_distance0[c0] == min_distance1[c1] || min_distance0[c0] == min_distance2[c2] || min_distance1[c1] == min_distance2[c2]);
    for (Vertex i = 0; i < numberVertices; ++i){
        if (min_distance2[i] != MAX_WEIGHT && min_distance1[i] != MAX_WEIGHT && min_distance0[i] != MAX_WEIGHT &&
                min_distance0[i] + min_distance1[i] + min_distance2[i] < distIntersect){
            intersect = i;
            distIntersect = min_distance0[i] + min_distance1[i] + min_distance2[i];
        }
    }

    if (intersect != -1){
        shared_ptr<treeNode> current;
        if (mapNode[intersect].expired()){
            current = shared_ptr<treeNode>(new treeNode(intersect));
            mapNode[intersect] = current;
        }
        else{
            current = mapNode[intersect].lock();
        }
        //Build the tree with origin1
        buildDownPath(adjList, min_distance1, origin1, intersect, inTree, mapNode);
        assert(cycle(root.get()));

        //Build the tree with origin2
        buildDownPath(adjList, min_distance2, origin2, intersect, inTree, mapNode);

        // Build the tree with origin0
        buildUpPath(current, adjList, min_distance0, origin0, inTree, mapNode);
        assert(cycle(root.get()));
        assert(sonof(root.get(), intersect));
        assert(sonof(root.get(), v1->v));
        assert(sonof(root.get(), v2->v));
    }else{
        if (r2 == root->v){
            //Build the tree with origin1
            buildDownPath(adjList, min_distance1, origin1, c1, inTree, mapNode);
            assert(sonof(mapNode[c1].lock().get(), v1->v));
            //Build the tree with origin2
            buildDownPath(adjList, min_distance2, origin2, c2, inTree, mapNode);
            assert(sonof(root.get(), v1->v));
            assert(sonof(root.get(), v2->v));
        }else if (r1 == root->v){
            //Build the tree with origin2
            buildDownPath(adjList, min_distance2, origin2, c2, inTree, mapNode);
            assert(sonof(mapNode[c2].lock().get(), v2->v));
            //Build the tree with origin1
            buildDownPath(adjList, min_distance1, origin1, c1, inTree, mapNode);
            assert(sonof(root.get(), v1->v));
            assert(sonof(root.get(), v2->v));
        }else{
            shared_ptr<treeNode> current = mapNode[c0].lock();

            //Build the tree with origin1
            if(r0 == v1->v){
                //Build the tree with origin2
                buildDownPath(adjList, min_distance2, origin2, c2, inTree, mapNode);
                assert(sonof(mapNode[c2].lock().get(), v2->v));
                //Build the tree with origin1
                buildDownPath(adjList, min_distance1, origin1, c0, inTree, mapNode);
            }else{
                //Build the tree with origin1
                buildDownPath(adjList, min_distance1, origin1, c1, inTree, mapNode);
                assert(sonof(mapNode[c1].lock().get(), v1->v));
                //Build the tree with origin2
                buildDownPath(adjList, min_distance2, origin2, c0, inTree, mapNode);
            }

            // Build the tree with origin0
            buildUpPath(current, adjList, min_distance0, origin0, inTree, mapNode);
            assert(sonof(root.get(),v1->v));
            assert(sonof(root.get(),v2->v));
            assert(sonof(root.get(), c0));
        }
    }
    assert(sonof(root.get(), v1->v));
    assert(sonof(root.get(), v2->v));
}

inline void opt_d3_parent(shared_ptr<treeNode> &node, const shared_ptr<treeNode> &root, const int numberVertices,
        const vector<map<Vertex, Weight> > &adjList, const vector<int> &terminalsMap,
        vector<weak_ptr<treeNode> > &mapNode, vector<bool> &inTree){
    vector<pair<Vertex, weak_ptr<treeNode> > > backup_children(node->children.begin(), node->children.end());
    for (vector<pair<Vertex, weak_ptr<treeNode> > >::iterator it1 = backup_children.begin(); it1 != backup_children.end(); ++it1){
        if (!node || node->parent == NULL || tle) return;
        map<Vertex, shared_ptr<treeNode> >::iterator tmp_it = node->children.find(it1->first);
        if (tmp_it == node->children.end()) break;
        shared_ptr<treeNode> v1 = tmp_it->second;
        shared_ptr<treeNode> v2 = node;

        node->parent->children.erase(node->v);
        deleteUpPath(node->parent, terminalsMap, inTree);

        // delete path starting with it1 to the first uselfull son
        node->children.erase(v1->v);
        while (v1->children.size() == 1 && terminalsMap[v1->v] == -1){
            inTree[v1->v] = false;
            v1 = v1->children.begin()->second;
        }
        v1->parent = NULL;
        v1->w = MAX_WEIGHT;

        // delete path starting with it2 to the first uselfull son
        while (v2->children.size() == 1 && terminalsMap[v2->v] == -1){
            inTree[v2->v] = false;
            v2 = v2->children.begin()->second;
        }
        v2->parent = NULL;
        v2->w = MAX_WEIGHT;

        if (node->v != v2->v) node.reset();

        opt_d3(root, v1, v2, numberVertices, adjList, terminalsMap, mapNode, inTree);
    }
}

inline void opt_d3_son(shared_ptr<treeNode> &node, const shared_ptr<treeNode> &root, const int numberVertices,
        const vector<map<Vertex, Weight> > &adjList, const vector<int> &terminalsMap,
        vector<weak_ptr<treeNode> > &mapNode, vector<bool> &inTree){
    vector<pair<Vertex, weak_ptr<treeNode> > > backup_children(node->children.begin(), node->children.end());
    for (vector<pair<Vertex, weak_ptr<treeNode> > >::iterator it1 = backup_children.begin(); it1 != backup_children.end(); ++it1){
        if (!node) break;
        for (vector<pair<Vertex, weak_ptr<treeNode> > >::iterator it2 = it1 + 1; it2 != backup_children.end(); ++it2){
            if (tle) return;
            if (!node) break;
            map<Vertex, shared_ptr<treeNode> >::iterator tmp_it = node->children.find(it1->first);
            if (tmp_it == node->children.end()) break;
            shared_ptr<treeNode> v1 = tmp_it->second;
            tmp_it = node->children.find(it2->first);
            if (tmp_it== node->children.end()) continue;
            shared_ptr<treeNode> v2 = tmp_it->second;

            // delete path starting with it1 to the first uselfull son
            node->children.erase(v1->v);
            while (v1->children.size() == 1 && terminalsMap[v1->v] == -1){
                inTree[v1->v] = false;
                v1 = v1->children.begin()->second;
            }
            v1->parent = NULL;
            v1->w = MAX_WEIGHT;

            // delete path starting with it2 to the first uselfull son
            node->children.erase(v2->v);
            while (v2->children.size() == 1 && terminalsMap[v2->v] == -1){
                inTree[v2->v] = false;
                v2 = v2->children.begin()->second;
            }
            v2->parent = NULL;
            v2->w = MAX_WEIGHT;

            treeNode* tmp = deleteUpPath(node.get(), terminalsMap, inTree);
            if (node->v != tmp->v) node.reset();

            opt_d3(root, v1, v2, numberVertices, adjList, terminalsMap, mapNode, inTree);

        }
    }
}

void apply_opt(Tree& T){
    vector<weak_ptr<treeNode> > mapNode(T.G.numberVertices);
    vector<bool> inTree(T.G.numberVertices, false);
    stack<Vertex> next_opt;
    {
        queue<shared_ptr<treeNode> > next;
        next.push(T.root);

        // We start the optimisation by the leaves
        // Fill next_opt with the order
        while (!next.empty()){
            shared_ptr<treeNode> current = next.front();
            next.pop();

            mapNode[current->v] = current;
            inTree[current->v] = true;

            for (map<Vertex, shared_ptr<treeNode> >::iterator it = current->children.begin(); it != current->children.end(); ++it){
                next.push(it->second);
                if (tle) return;
            }
            next_opt.push(current->v);
        }
    }

    // apply optimisation following the next_opt order
    while (!next_opt.empty()){
        weak_ptr<treeNode> current = mapNode[next_opt.top()];
        next_opt.pop();
        if (!current.expired()){
            shared_ptr<treeNode> node = current.lock();

            // Optimisation of the path to the root
            if (tle) return;
            if (node && (node->children.size() > 2 || T.terminalsMap[node->v] != -1)){
                Weight w = T.size();
                opt_d2(node, T.G.numberVertices, T.G.adjList, T.terminalsMap, mapNode , inTree);
                assert(w>= T.size());
            }

            // Optimisation for node of degree 3 or more
            if (tle) return;
            if (node->children.size() > 1){
                Weight w1 = T.size();
                opt_d3_son(node, T.root, T.G.numberVertices, T.G.adjList, T.terminalsMap, mapNode, inTree);
                assert(w1>= T.size());
            }

//            // Optimisation of terminal node in a path
//            if (tle) return;
//            if (node && (node->children.size() == 1 && T.terminalsMap[node->v] != -1)){
//                Weight w = T.size();
//                opt_d3_parent(node, T.root, T.G.numberVertices, T.G.adjList, T.terminalsMap, mapNode , inTree);
//                assert(w>= T.size());
//            }
            assert(cycle(T.root.get()));
        }
    }
}

void full_d3(Tree& T){
    vector<weak_ptr<treeNode> > mapNode(T.G.numberVertices);
    vector<bool> inTree(T.G.numberVertices, false);
    vector<Vertex> next_opt;
    next_opt.reserve(T.G.numberVertices);
    {
        queue<shared_ptr<treeNode> > next;
        next.push(T.root);

        while (!next.empty()){
            shared_ptr<treeNode> current = next.front();
            next.pop();

            mapNode[current->v] = current;
            inTree[current->v] = true;

            for (map<Vertex, shared_ptr<treeNode> >::iterator it = current->children.begin(); it != current->children.end(); ++it){
                next_opt.push_back(it->first);
                next.push(it->second);
                if (tle) return;
            }
        }
    }

    for (vector<Vertex>::iterator it1 = next_opt.begin(); it1 != next_opt.end(); ++it1){
        for (vector<Vertex>::iterator it2 = it1 + 1; it2 != next_opt.end(); ++it2){
            if (tle) return;
            if (!mapNode[*it1].expired()){
                shared_ptr<treeNode> v1 = mapNode[*it1].lock();
                if (v1->children.size() > 1 || T.terminalsMap[*it1] != -1){
                    if (!mapNode[*it2].expired()){
                        shared_ptr<treeNode> v2 = mapNode[*it2].lock();
                        if (v2->children.size() > 1 || T.terminalsMap[*it2] != -1){
                            opt_d3(T.root, v1, v2, T.G.numberVertices, T.G.adjList, T.terminalsMap, mapNode, inTree);
                        }
                    }
                }
            }

        }
    }
}

pair<Tree, Weight> full_opt(Graph &G, vector<int> &terminalsMap, vector<Vertex> &terminals){
    Tree T(G, terminalsMap, terminals);
    Weight wf = MAX_WEIGHT;
    for (vector<Vertex>::iterator it = terminals.begin(); it != terminals.end(); ++it){
        if (tle) break;
        Tree tmp = incrementalDijks3(G, *it, terminalsMap, terminals);
        Weight w = tmp.pruneLeaves();
        //cout << "init " << w << endl;
        Weight oldw;
        do{
            oldw = w;
            apply_opt(tmp);
            w = tmp.pruneLeaves();
            //cout << w << endl;
            assert(w<=oldw);
            if (tle) break;
        } while (oldw != w);
        //cout << "***" << endl;
        if (wf > w){
            //cout << "================= " << w << endl;
            wf = w;
            T.root = tmp.root;
        }
    }
    return {T, wf};
}
