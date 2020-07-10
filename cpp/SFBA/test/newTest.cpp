#include <bits/stdc++.h>
#include "../Topology.h"
#include "../SFBA.hpp"
#include "../Stellar.hpp"

using namespace std;

int nodes = 200;
int edges = nodes * 4;


int main() {
    Topology tp;

    tp.Random(nodes, edges, 10, 2);
    tp.GenerateAllPairShortestPath();

    SFBA sfba(tp);
    Stellar stellar(tp);

    sfba.fraction_corrupted = 0;

    sfba.Bootstrap();
    sfba.Run(1);
}

