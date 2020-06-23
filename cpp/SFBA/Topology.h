//
// Created by misclicked on 2020/6/21.
//

#ifndef SFBA_TOPOLOGY_H
#define SFBA_TOPOLOGY_H

#include <string>
#include <vector>

class Topology {    //network Topology class, undirected
public:
    int MIN_LATENCY_MS = 5;
    int MAX_LATENCY_MS = 100;

    int nodes, edges;

    int **AdjList;

    struct Edge {
        int u, v, w;
    };

    struct Node {
        int v, w;
    };

    std::vector<Edge> Edges;
    std::vector<std::vector<Node> > G;

    Topology() {};

    void FromFile(std::string Filename, bool containsWeight = false);

    void Random(int nodes, int edges);

    void Save(std::string Filename, bool replace = true);

    void GenerateAllPairShortestPath();

private:
    void addEdge(int u, int v, int w);
};


#endif //SFBA_TOPOLOGY_H
