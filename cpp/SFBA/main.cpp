#include <bits/stdc++.h>
#include "Topology.h"
#include "Config.hpp"

using namespace std;

void SimulateSFBA_OnlyGetAcceptQuorum(int rounds, Config &config) {
    cout << "SFBA" << endl;
    for (int i = 0; i < rounds; i++) {
        auto committee = config.getRandomNodes(30);
        int innerLatency = config.innerCommunicateTime(committee);
        cout << config.StartGatherWatcher(config.getRandomNodes(100), committee) + innerLatency << endl;
    }
}

void SimulateStellar(int rounds, Config &config, std::vector<int> stellarCommittee) {
    cout << "Stellar" << endl;
    int innerLatency = config.innerCommunicateTime(stellarCommittee);
    for (int i = 0; i < rounds; i++) {
        cout << config.StartGatherWatcher(config.getRandomNodes(200), stellarCommittee) + innerLatency << endl;
    }
}

int main() {
    Topology tp;
    tp.Random(200, 800, 5, 30);

    Config SFBA(tp), Stellar(tp);

    SFBA.RandomizeSlices(30, 60, 2, 6);

    auto committee = Stellar.getRandomNodes(30);   //stellar committee;

    Stellar.LetWeTrustThese(committee, 30, 60, 3, 4, 3, 3);

    SimulateSFBA_OnlyGetAcceptQuorum(100, SFBA);
    SimulateStellar(100, Stellar, committee);
}

