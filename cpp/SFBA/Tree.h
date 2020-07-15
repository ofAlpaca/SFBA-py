//
// Created by misclicked on 2020/6/21.
//

#ifndef SFBA_TREE_H
#define SFBA_TREE_H

#include <vector>

class Tree {
public:
    static std::vector<std::vector<int>> Generate(int n);

private:
};

class Dij {
    int *rank, *parent, n;
public:
    void Initialize(int n) {
        rank = new int[n];
        parent = new int[n];

        this->n = n;
        for (int i = 0; i < n; i++) {
            parent[i] = i;
        }
    }

    int Find(int x) {
        return parent[x] == x ? x : parent[x] = Find(parent[x]);
    }

    void Union(int x, int y) {
        int xset = Find(x);
        int yset = Find(y);

        if (xset == yset)
            return;

        if (rank[xset] < rank[yset]) {
            parent[xset] = yset;
        } else if (rank[xset] > rank[yset]) {
            parent[yset] = xset;
        } else {
            parent[yset] = xset;
            rank[xset] = rank[xset] + 1;
        }
    }

    bool IsSame(int a, int b) {
        return Find(a) == Find(b);
    }
};


#endif //SFBA_TREE_H
