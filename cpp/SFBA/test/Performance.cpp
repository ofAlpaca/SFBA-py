#include <bits/stdc++.h>
#include "../Topology.h"
#include "../Config.hpp"

using namespace std;

const int nodes = 1000;
const int edges = nodes * 4;
const int min_slices = nodes / 30;
const int max_slices = nodes / 20;
const int min_slice_member = 2;
const int max_slice_member = 3;
const int min_trust_slice_member = 2;
const int max_trust_slice_number = 2;
const int min_slice_redundant = 2;
const int max_slice_redundant = max_slice_member - max_trust_slice_number;
const int baseCommitteeCnt = nodes / 4;
const int rounds = 20;

void SimulateSFBA_OnlyGetAcceptQuorum(int rounds, int committeeSize, Config &config) {
    cout << "SFBA node:" << nodes << endl;
    double ret = 0;
    for (int i = 0; i < rounds; i++) {
        auto committee = config.getRandomNodes(committeeSize);
        int innerLatency = config.innerCommunicateTime(committee);
        //cout << config.StartGatherWitness(committee, 0.5) + innerLatency << endl;
        ret += config.StartGatherWitness(committee, 0.5) + innerLatency;
    }
    cout << fixed << ret / rounds << endl;
}

void SimulateStellar(int rounds, Config &config, std::vector<int> stellarCommittee) {
    cout << "Stellar node:" << nodes << endl;
    double ret = 0;
    int innerLatency = config.innerCommunicateTime(stellarCommittee);
    for (int i = 0; i < rounds; i++) {
        ret += config.StartGatherWitness(stellarCommittee, 1) + innerLatency;
    }
    cout << fixed << ret / rounds << endl;
}


int main() {
    Topology tp;
    tp.Random(nodes, edges, 5, 30); //節點數,邊數,邊權重最小值,邊權重最大值

    Config SFBA(tp), Stellar(tp);

    SFBA.RandomizeSlices(min_slices, max_slices, min_slice_member,
                         max_slice_member); //每個節點最少選幾個slice,每個節點最多選幾個slice,每個slice最少成員,每個slice最多成員

    auto committee = Stellar.getRandomNodes(baseCommitteeCnt);   //stellar committee;

    Stellar.LetWeTrustThese(committee, min_slices, max_slices, min_trust_slice_member, max_trust_slice_number,
                            min_slice_redundant, max_slice_redundant);
    //Stellar固定成員,每個節點最少選幾個slice,每個節點最多選幾個slice,每個slice最少相信幾個stellar成員,每個slice最多相信幾個stellar成員,每個slice最少相信幾個其他成員,每個slice最多相信幾個其他成員,

    SimulateSFBA_OnlyGetAcceptQuorum(10, baseCommitteeCnt, SFBA);   //模擬SFBA 100輪
    SimulateStellar(10, Stellar, committee);      //模擬Stellar 100輪
}

