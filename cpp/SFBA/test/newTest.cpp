#include <bits/stdc++.h>
#include "../Topology.h"
#include "../SFBA.hpp"
#include "../Stellar.hpp"

using namespace std;


double corrupted = 0.0;
double witnessFraction = 0.01;
int hi = 0;

int main() {
    freopen("Corrupted.txt", "w", stdout);
    freopen("debug.txt", "w", stderr);


    for (int nodes = 50; nodes <= 1000; nodes += 50) {
        Topology tp;
        tp.Random(nodes, 1, 8, 0);
        tp.GenerateAllPairShortestPath();
        SFBA sfba(tp, nodes);

        sfba.corruptedNode = 0;
        sfba.witnessSize = 5;


        sfba.Bootstrap();
        sfba.Run(50);
        cout << fixed << nodes << "\t" << sfba.getAvaCFT() << endl;
    }
}