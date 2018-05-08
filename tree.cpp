#include "tree.h"

using namespace std;

shared_ptr<treeNode> treeNode::addChild(Vertex v, Weight w){
    shared_ptr<treeNode> child(new treeNode(v,w,this));
    children.insert({v,child});
    return child;
}

shared_ptr<treeNode> treeNode::addChild(shared_ptr<treeNode> N){
    children.insert(pair<Vertex, shared_ptr<treeNode> >(N->v,N));
    return N;
}

void treeNode::print(){
    cout << "Node: " << v;
    if (parent) cout << " ,parent: " << parent->v << " weight: " << w;
    cout << endl;
}

void treeNode::printSubTree(){
    stack<treeNode> next;
    next.push(*this);
    while (!next.empty()){
        treeNode current = next.top();
        next.pop();
        for (map<Vertex, shared_ptr<treeNode> >::iterator it = current.children.begin(); it != current.children.end(); ++it){
            cout << current.v + 1 << " " << it->first + 1 << endl;
            next.push(*it->second);
        }
    }
}

int treeNode::pruneLeaves(const vector<int>& terminalsMap){
    int tmp=0;
    map<Vertex, shared_ptr<treeNode> > backup = children;
    for (map<Vertex, shared_ptr<treeNode> >::iterator it = backup.begin(); it != backup.end(); ++it){
        tmp += it->second->pruneLeaves(terminalsMap);
    }
    if (children.empty() && terminalsMap[v] == -1){
        parent->children.erase(v);
    } else {
        if (parent)
            tmp += w;
    }
    return tmp;
}

void Tree::print(){
    root->printSubTree();
}

bool Tree::isTree(){
    stack<treeNode> next;
    set<Vertex> seen;
    next.push(*root);
    seen.insert(root->v);
    while (!next.empty()){
        treeNode current = next.top();
        next.pop();
        for (map<Vertex, shared_ptr<treeNode> >::iterator it = current.children.begin(); it != current.children.end(); ++it){
            if (!seen.insert(it->first).second) return false;
            next.push(*it->second);
        }
    }
    return true;
}

bool Tree::isConsistent(){
    stack<shared_ptr<treeNode> > next;
    next.push(root);
    while(!next.empty()){
        shared_ptr<treeNode> current = next.top();
        next.pop();
        for (map<Vertex, shared_ptr<treeNode> >::iterator it = current->children.begin(); it != current->children.end(); ++it){
            if (it->second->parent != current.get()) return false;
            next.push(it->second);
        }
    }
    return true;
}

ListGraph Tree::convertToGraph(){
    ListGraph list_G(G);
    stack<shared_ptr<treeNode> > next;
    next.push(root);
    while (!next.empty()){
        shared_ptr<treeNode> current = next.top();
        next.pop();
        for (map<Vertex, shared_ptr<treeNode> >::iterator it = current->children.begin(); it != current->children.end(); ++it){
            list_G.insertEdge(edge(current->v,it->first));
            next.push(it->second);
        }
    }
    return list_G;
}

Tree Tree::subTree(treeNode N){
    N.w = MAX_WEIGHT;
    N.parent = NULL;
    return Tree(G,N);
}

void Tree::replaceNode(treeNode N, Tree &T){
    if (N.v != T.root->v) throw "Mismach vertex in replaceNode try to replace " + to_string(N.v) + " with " + to_string(T.root->v);
    N.children = T.root->children;
}

void Tree::pruneRoot(){
    if (root->children.size() == 1 && terminalsMap[root->v] == -1){
        root = root->children.begin()->second;
        root->parent=NULL;
        root->w=MAX_WEIGHT;
        pruneRoot();
    }
}

Weight Tree::pruneLeaves(){
    Weight tmp = 0;
    stack<shared_ptr<treeNode> > next;
    vector<shared_ptr<treeNode> > to_clean;
    next.push(root);
    while (!next.empty()){
        shared_ptr<treeNode> current = next.top();
        next.pop();
        for (map<Vertex, shared_ptr<treeNode> >::iterator it = current->children.begin(); it != current->children.end(); ++it){
            if (it->second->children.size() == 0 && terminalsMap[it->first] == -1)
                to_clean.push_back(it->second);
            next.push(it->second);
            tmp += it->second->w;
        }
    }

    for (vector<shared_ptr<treeNode> >::iterator it = to_clean.begin(); it != to_clean.end(); ++it){
        treeNode* current = it->get();
        while (current->children.size() == 0 && terminalsMap[current->v] == -1 && current->parent != NULL){
            tmp -= current->w;
            treeNode* next = current->parent;
            current->parent->children.erase(current->v);
            current = next;
        }
    }
    return tmp;
}

int Tree::size(){
    int tmp = 0;
    stack<shared_ptr<treeNode> > next;
    next.push(root);
    while (!next.empty()){
        shared_ptr<treeNode> current = next.top();
        next.pop();
        for (map<Vertex, shared_ptr<treeNode> >::iterator it = current->children.begin(); it != current->children.end(); ++it){
            next.push(it->second);
            tmp += it->second->w;
        }
    }

    return tmp;
}
