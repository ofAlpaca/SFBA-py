#include <bits/stdc++.h>
#include "../Topology.h"
#include "../SFBA.hpp"
#include "../Stellar.hpp"

using namespace std;


double corrupted = 0.0;
double witnessFraction = 0.01;
int hi = 0;

int main() {
    freopen("PerformenceStllar.txt", "w", stdout);
    freopen("debug.txt", "w", stderr);

#pragma omp parallel for default(none)
    for (int nodes = 100; nodes <= 1000; nodes += 100) {
        Topology tp;
        tp.Random(nodes, nodes * 3, 8, 0);
        tp.GenerateAllPairShortestPath();
        Stellar stellar(tp, nodes);

        stellar.fraction_corrupted = corrupted;
        stellar.witnessFraction = witnessFraction;


        stellar.Bootstrap();
        stellar.Run(1);
        cout << nodes << "\t" << stellar.getAvaCFT() << endl;
    }
}