#ifndef EDGE_H
#define EDGE_H

#include "graph.h"

using namespace std;

class edge{
    private:
    public:
        Vertex v1;
        Vertex v2;
        void print() const {cout << "( " << v1 + 1 << ", " << v2 + 1 << " )" << endl;};
        edge():v1{0},v2{0}{};
        edge(Vertex v1, Vertex v2):v1{v1},v2{v2}{};
        void setV1(Vertex v){v1 = v;};
        void setV2(Vertex v) {v2 = v;};
        edge reverse(){return edge(v2,v1);};
        friend bool operator<(const edge e1, const edge e2);
        friend bool operator==(const edge e1, const edge e2);
};;

#endif /* EDGE_H */
