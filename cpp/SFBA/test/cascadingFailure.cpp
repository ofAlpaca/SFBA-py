#include <bits/stdc++.h>
#include "../Topology.h"
#include "../SFBA.hpp"
#include "../Stellar.hpp"

using namespace std;


double corrupted = 0.0;
double witnessFraction = 0.01;
int hi = 0;

int main() {
    freopen("caf.txt", "w", stdout);
    //freopen("debug.txt", "w", stderr);
    Topology tp;
    tp.Random(1000, 2000, 8, 0);
    tp.GenerateAllPairShortestPath();


    Stellar sfba(tp, 200);

    sfba.fraction_corrupted = corrupted;
    sfba.witnessFraction = witnessFraction;


    auto committee = sfba.getRandomCommittee(30);
    vector<int> shake;
    for (int i = 0; i < 200; i++)shake.push_back(i);
    sfba.LetWeTrustThese(committee, 5, 10, 2, 2, 2, 2, 2, 8);

    for (double per = 0.05; per < 1.01; per += 0.05) {
        cout << per << endl;
        for (int i = 0; i < 100; i++) {
            random_shuffle(shake.begin(), shake.end());
            vector<int> down;
            for (int j = 0; j < 200 * per; j++) {
                down.push_back(shake[j]);
            }
            cout << sfba.CascadingFailure(committee, down) << "\t";
        }
        cout << endl;
    }
}