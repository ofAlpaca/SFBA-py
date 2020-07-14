//
// Created by misclicked on 2020/7/13.
//

#ifndef SFBA_DIJ_HPP
#define SFBA_DIJ_HPP

class Dij {
    int n;
    int *parent;
public:
    Dij() {};

    void init(int n) {
        this->n = n;
        parent = new int[n];
        for (int i = 0; i < n; i++)
            parent[i] = i;
    }

    int find(int n) {
        return parent[n] == n ? parent[n] : parent[n] = find(parent[n]);
    }

    void uni(int x, int y) {
        parent[find(x)] = find(y);
    }

    bool isSame(int x, int y) {
        return find(x) == find(y);
    }
};

#endif //SFBA_DIJ_HPP
