#pragma once

#include <csignal>
#include <queue>

#include "tree.h"
#include "init.h"

extern volatile sig_atomic_t tle;

void signalHandler( int );

void apply_opt(Tree &T);
void full_d3(Tree& T);

void opt(Tree &T);

pair<Tree, Weight> full_opt(Graph &G, vector<int> &terminalsMap, vector<Vertex> &terminals);
