#include <bits/stdc++.h>
#include "../Topology.h"
#include "../SFBA.hpp"
#include "../Stellar.hpp"

using namespace std;

int nodes = 200;
int edges = nodes * 4;

double corrupted = 0.00;
int hi = 0;

int main() {
    freopen("Stellar_Fail_050.txt", "w", stdout);
    Topology tp;

    tp.Random(nodes, edges, 10, 2);
    tp.GenerateAllPairShortestPath();

    Stellar stellar(tp);

    stellar.fraction_corrupted = corrupted;

    stellar.Bootstrap();
    stellar.Run(1);
}

