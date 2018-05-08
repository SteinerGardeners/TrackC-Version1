#include <cstdlib>
#include <iostream>
#include <fstream>
#include <csignal>
#include <string>
#include <random>
#include <chrono>

#include <unistd.h>


#include "graph.h"
#include "edge.h"
#include "tree.h"
#include "listgraph.h"
#include "init.h"
#include "opt.h"

using namespace std;

Vertex center(Graph &G){
    G.build_distTerminalsVertices();
    Vertex center = 0;
    Weight center_dist = MAX_WEIGHT;
    for (Vertex v = 0; v < G.numberVertices; ++v){
        Weight dist = 0;
        for (int t = 0; t < G.numberTerminals; ++t){
            dist = max(G.DistTerminalsVertices[t][v],dist);
        }
        if (dist < center_dist){
            center_dist = dist;
            center = v;
        }
    }
    return center;
}

int main(int argc, char** argv) {
    // register signal SIGINT and signal handler
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    unsigned int seed{0};
    if (argc == 3){
        seed = atoi(argv[2]);
    }
    else{
        std::random_device rand_dev;
        seed = rand_dev();
    }
    std::mt19937 generator(seed);

    Graph G(cin);
//    Tree T(G);
//    Weight w, oldw; //wf = MAX_WEIGHT;
//    for (vector<Vertex>::iterator it = G.terminals.begin(); it != G.terminals.end(); ++it){
//        if (tle) break;
//        Tree tmp = incrementalDijks3(G, *it, G.terminalsMap, G.terminals);
//        w = tmp.pruneLeaves();
//        //cout << "init " << w << endl;
//        do{
//            if (tle) break;
//            oldw = w;
//            apply_opt(tmp);
//            w = tmp.pruneLeaves();
//            //cout << w << endl;
//            assert(w<=oldw);
//        } while (oldw != w);
//        //cout << "***" << endl;
//        if (wf > w){
//            //cout << "================= " << w << endl;
//            wf = w;
//            T.root = tmp.root;
//        }
//    }
//    w = wf;
//    do{
//        if (tle) break;
//        oldw = w;
//        full_d3(T);
//        w = T.pruneLeaves();
//        //cout << w << endl;
//        assert(w<=oldw);
//    } while (oldw != w);
//    cout << "VALUE " << wf << endl;

//    Vertex start = G.terminals[0];
//    Tree T = incrementalDijks3(G, start, G.terminalsMap, G.terminals);
//    T.pruneRoot();
//    T.pruneLeaves();
//    do{
//        if (tle) break;
//        oldw = w;
//        apply_opt(T);
//        w = T.pruneLeaves();
//        //cout << w << endl;
//        assert(w<=oldw);
//    } while (oldw != w);
//    cout << "VALUE " << T.pruneLeaves() << endl;
//    if (G.numberVertices * G.numberTerminals < 64000000){
//        start = center(G);
//        cout << "center " << start << endl;
//    }
//    T.root = incrementalDijks3(G, start, G.terminalsMap, G.terminals).root;
//    T.pruneRoot();
//    T.pruneLeaves();
//    do{
//        if (tle) break;
//        oldw = w;
//        apply_opt(T);
//        w = T.pruneLeaves();
//        //cout << w << endl;
//        //assert(w<=oldw);
//    } while (oldw != w);
    pair<Tree, Weight> anwser = full_opt(G, G.terminalsMap, G.terminals);
    cout << "VALUE " << anwser.second << endl;
    anwser.first.print();
    return 0;

}

