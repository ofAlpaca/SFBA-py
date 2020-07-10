//
// Created by misclicked on 2020/6/21.
//

#include <iostream>
#include <set>
#include <fstream>
#include <algorithm>
#include <queue>
#include <climits>
#include "Topology.h"
#include "Tree.h"
#include "Global.h"

void Topology::FromFile(std::string Filename, int min_latency_ms, int max_latency_ms, bool containsWeight) {
    std::fstream file;
    std::ios_base::openmode option = std::ios::in;

    std::uniform_int_distribution<> r_latency(min_latency_ms, max_latency_ms);

    file.open(Filename.c_str(), option);
    if (!file)
        throw std::runtime_error("no such file");

    this->edges = 0;

    int u, v, w, max_n = 0;
    Edges.clear();
    if (containsWeight)
        while (file >> u >> v >> w) {
            Edges.push_back({u, v, w});
            max_n = std::max({max_n, u, v});
        }
    else
        while (file >> u >> v) {
            Edges.push_back({u, v, r_latency(Global::rng.get())});
            max_n = std::max({max_n, u, v});
        }

    this->nodes = max_n + 1;
    this->edges = Edges.size();
    G.resize(this->nodes);

    for (auto it:Edges) {
        G[it.u].push_back({it.v, it.w});
        G[it.v].push_back({it.u, it.w});
    }
}

void Topology::Random(int nodes, int edges, int mean_latency_ms, int stddev_latency_ms) {
    this->nodes = nodes;
    this->edges = 0;

    std::normal_distribution<> r_latency(mean_latency_ms, stddev_latency_ms);
    std::uniform_int_distribution<> r_node(0, nodes - 1);
    std::vector<std::vector<int>> t = Tree::Generate(nodes);

    G.resize(nodes);
    Edges.clear();

    for (int u = 0; u < t.size(); u++) {
        for (auto v:t[u]) {
            int weight = std::max((int) std::round(r_latency(Global::rng.get())), 1);
            addEdge(u, v, weight);
            this->edges++;
        }
    }

    int remain = edges - this->edges;

    std::set<std::pair<int, int>> se;

    for (int i = 0; i < remain;) {
        int a = r_node(Global::rng.get());
        int b = r_node(Global::rng.get());
        int weight = std::max((int) std::round(r_latency(Global::rng.get())), 1);
        if (!se.count({std::min(a, b), std::max(a, b)})) {
            addEdge(a, b, weight);
            this->edges++;
            se.insert({std::min(a, b), std::max(a, b)});
            i++;
        }
    }
}

void Topology::addEdge(int u, int v, int w) {
    Edges.push_back({u, v, w});
    Edges.push_back({v, u, w});
    G[u].push_back({v, w});
    G[v].push_back({u, w});
}

void Topology::Save(std::string Filename, bool replace) {
    std::fstream file;
    std::ios_base::openmode option = std::ios::out;
    if (replace)
        option |= std::ios::trunc;

    file.open(Filename.c_str(), option);

    if (!replace && file) {
        throw std::runtime_error("file already existed");
    }

    for (int i = 0; i < edges; i++) {
        file << Edges[i].u << "\t" << Edges[i].v << "\t" << Edges[i].w << std::endl;
    }

    file.close();
}

void Topology::GenerateAllPairShortestPath() {
    AdjList = new int *[nodes];
    for (int i = 0; i < nodes; i++) {
        AdjList[i] = new int[nodes];
        for (int j = 0; j < nodes; j++)
            AdjList[i][j] = INT_MAX;
    }


#pragma omp parallel for default(none)
    for (int i = 0; i < nodes; i++) {
        AdjList[i][i] = 0;
        std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<std::pair<int, int>>> pq;
        pq.push({0, i});
        while (pq.size()) {
            std::pair<int, int> now = pq.top();
            pq.pop();
            if (AdjList[i][now.second] < now.first)continue;
            for (auto it:G[now.second]) {
                if (AdjList[i][it.v] > now.first + it.w) {
                    AdjList[i][it.v] = now.first + it.w;
                    pq.push({AdjList[i][it.v], it.v});
                }
            }
        }
    }
}
