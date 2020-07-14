#include <bits/stdc++.h>
#include "../Topology.h"
#include "../SFBA.hpp"
#include "../Stellar.hpp"

using namespace std;

int nodes = 200;
int edges = nodes * 4;

int hi = 0;

int main() {
    freopen("debug2.txt", "w", stdout);
    freopen("Witness.txt", "w", stderr);

    for (int stddev = 0; stddev <= 20; stddev++) {
        Topology tp;

        tp.Random(nodes, edges, 20, stddev);
        tp.GenerateAllPairShortestPath();
        for (double corrupted = 0.0; corrupted <= 1.01; corrupted += 0.05) {
            for (double witnessFraction = 0.0; witnessFraction <= 1.01; witnessFraction += 0.05) {
                SFBA sfba(tp);

                sfba.fraction_corrupted = corrupted;
                sfba.witnessFraction = witnessFraction;


                sfba.Bootstrap();
                sfba.Run(5);
                cerr << stddev << "\t" << witnessFraction << "\t" << corrupted << "\t" << sfba.getAvaCFT() << endl;
            }
        }

    }
}
