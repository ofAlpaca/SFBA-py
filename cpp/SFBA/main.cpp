#include <bits/stdc++.h>
#include "Topology.h"
#include "Config.hpp"

using namespace std;

void SimulateSFBA_OnlyGetAcceptQuorum(int rounds, Config &config) {
    cout << "SFBA" << endl;
    for (int i = 0; i < rounds; i++) {
        auto committee = config.getRandomNodes(100);
        int innerLatency = config.innerCommunicateTime(committee);
        cout << config.StartGatherWatcher(config.getRandomNodes(10), committee) + innerLatency << endl;
    }
}

void SimulateStellar(int rounds, Config &config, std::vector<int> stellarCommittee) {
    cout << "Stellar" << endl;
    int innerLatency = config.innerCommunicateTime(stellarCommittee);
    for (int i = 0; i < rounds; i++) {
        cout << config.StartGatherWatcher(config.getRandomNodes(1000), stellarCommittee) + innerLatency << endl;
    }
}

int main() {
    Topology tp;
    tp.Random(1000, 4000, 5, 30); //節點數,邊數,邊權重最小值,邊權重最大值

    Config SFBA(tp), Stellar(tp);

    SFBA.RandomizeSlices(30, 60, 2, 10); //每個節點最少選幾個slice,每個節點最多選幾個slice,每個slice最少成員,每個slice最多成員

    auto committee = Stellar.getRandomNodes(100);   //stellar committee;

    Stellar.LetWeTrustThese(committee, 30, 60, 3, 4, 3, 3);
    //Stellar固定成員,每個節點最少選幾個slice,每個節點最多選幾個slice,每個slice最少相信幾個stellar成員,每個slice最多相信幾個stellar成員,每個slice最少相信幾個其他成員,每個slice最多相信幾個其他成員,

    SimulateSFBA_OnlyGetAcceptQuorum(10, SFBA);   //模擬SFBA 100輪
    SimulateStellar(10, Stellar, committee);      //模擬Stellar 100輪
}

