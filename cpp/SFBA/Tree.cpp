//
// Created by misclicked on 2020/6/21.
//

#include <cstdlib>
#include <iostream>
#include <random>
#include "Tree.h"
#include "Global.h"

std::vector<std::vector<int>> Tree::Generate(int n) {
    std::uniform_int_distribution<> r_node(0, n - 1);

    Dij se;
    se.Initialize(n);
    std::vector<std::vector<int>> G;
    G.resize(n);

    int longNeck = n / 5;

    int i = 0;

    for (int k = 0; k < longNeck; k++) {
        G[i].push_back(i + 1);
        G[i + 1].push_back(i);
        se.Union(i, i + 1);
        i++;
    }
    for (int k = 0; k < longNeck; k++) {
        G[i].push_back(i + 1);
        G[i + 1].push_back(i);
        se.Union(i, i + 1);
        i++;
    }

    for (; i < n - 1;) {
        int a = r_node(Global::rng.get());
        int b = r_node(Global::rng.get());
        if (!se.IsSame(a, b)) { //check if already in same tree
            se.Union(a, b);
            G[a].push_back(b);  //add edge a->b
            G[b].push_back(a);  //add edge b->a
            i++;
        }
    }
    return G;
}