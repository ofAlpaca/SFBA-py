#include <bits/stdc++.h>
#include "../Topology.h"
#include "../SFBA.hpp"
#include "../Stellar.hpp"

using namespace std;


double corrupted = 0.0;
double witnessFraction = 0.01;
int hi = 0;

int main() {
    freopen("Corrupted100.txt", "w", stdout);
    freopen("debug100.txt", "w", stderr);
    Topology tp;
    tp.Random(1000, 2000, 8, 0);
    tp.GenerateAllPairShortestPath();


#pragma omp parallel for default(none)
    for (int nodes = 0; nodes <= 200; nodes += 5) {
        SFBA sfba(tp, 200);

        sfba.corruptedNode = nodes;
        sfba.witnessSize = 100;


        sfba.Bootstrap();
        sfba.Run(55);
        cout << fixed << (double) nodes / 200 << "\t" << sfba.getAvaCFT() << endl;
    }
}