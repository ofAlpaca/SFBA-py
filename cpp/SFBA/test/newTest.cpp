#include <bits/stdc++.h>
#include "../Topology.h"
#include "../SFBA.hpp"
#include "../Stellar.hpp"

using namespace std;

int nodes = 200;
int edges = nodes * 4;

double corrupted = 0.0;
double witnessFraction = 0.01;
int hi = 0;

int main() {
    freopen("RandomAttack.txt", "w", stdout);
    freopen("debug.txt", "w", stderr);
    Topology tp;

    tp.Random(nodes, edges, 20, 10);
    tp.GenerateAllPairShortestPath();

    while (corrupted <= 1.01) {
        SFBA sfba(tp);

        sfba.fraction_corrupted = corrupted;
        sfba.witnessFraction = witnessFraction;


        sfba.Bootstrap();
        sfba.Run(3);
        cerr << corrupted << "\t" << sfba.getAvaCFT() << endl;
        corrupted += 0.05;
    }
}
