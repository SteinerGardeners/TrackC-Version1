#include "edge.h"


bool operator<(const edge e1, const edge e2){
    return (e1.v1 <= e2.v1) && (e1.v1 < e2.v1 || e1.v2 < e2.v2);
}

bool operator==(const edge e1, const edge e2){
    return (e1.v1 == e2.v1) && (e1.v2 == e2.v2);
}
