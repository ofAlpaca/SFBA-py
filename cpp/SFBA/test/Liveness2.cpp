#include <bits/stdc++.h>
#include "../Topology.h"
#include "../Config.hpp"

using namespace std;

const int nodes = 200;
const int edges = nodes * 4;
const int min_slices = 15;
const int max_slices = 17;
const int min_slice_member = 2;
const int max_slice_member = 7;
const int min_trust_slices_member = 2;
const int max_trust_slices_member = 3;
const int min_trust_slice_number = 2;
const int max_trust_slice_number = 3;
const int min_slice_redundant = 2;
const int max_slice_redundant = max_slices - max_trust_slice_number;
const int baseCommitteeCnt = nodes / 4;
const int Timeout = 5000;

float fraction_corrupted = 0.05;

void SimulateSFBA_OnlyGetAcceptQuorum(int rounds, int committeeSize, Config &config) {
    fraction_corrupted = 0.05;
    cout << "SFBA node:" << nodes << endl;
    long long time = 0, blockHeight = 0;
    for (int i = 0; i < rounds; i++) {
        config.ToggleAllNodeUp();
        config.ToggleRandomNodeDown(fraction_corrupted);
        if (i > rounds / 2)fraction_corrupted = 0.3;
        auto committee = config.getRandomNodes(committeeSize);
        int innerLatency = config.innerCommunicateTime(committee);
        int outerLatency = config.StartGatherWitness(committee, 0.5);
        if (outerLatency == INT_MAX) {
            i--;
            time += (long long) Timeout + innerLatency;
            config.splitSlicesRandom();
        } else {
            time += (long long) outerLatency + innerLatency;
            blockHeight++;
        }
        cout << time << "\t" << blockHeight << endl;
    }
}

void SimulateStellar(int rounds, Config &config, std::vector<int> stellarCommittee) {
    fraction_corrupted = 0.05;
    cout << "Stellar node:" << nodes << endl;
    long long time = 0, blockHeight = 0;
    int innerLatency = config.innerCommunicateTime(stellarCommittee);
    for (int i = 0; i < rounds; i++) {
        config.ToggleAllNodeUp();
        if (i > rounds / 2)fraction_corrupted = 0.3;
        config.ToggleRandomNodeDown(fraction_corrupted);
        int outerLatency = config.StartGatherWitness(stellarCommittee, 1 - fraction_corrupted);
        if (outerLatency > Timeout)
            time += Timeout + innerLatency;
        else {
            time += (long long) outerLatency + innerLatency;
            blockHeight++;
        }
        cout << time << "\t" << blockHeight << endl;
    }
}


int main() {
    Topology tp;
    tp.Random(nodes, edges, 5, 30); //節點數,邊數,邊權重最小值,邊權重最大值

    Config SFBA(tp), Stellar(tp);

    SFBA.RandomizeSlices(min_slices, max_slices, min_slice_member,
                         max_slice_member); //每個節點最少選幾個slice,每個節點最多選幾個slice,每個slice最少成員,每個slice最多成員

    auto committee = Stellar.getRandomNodes(baseCommitteeCnt);   //stellar committee;

    Stellar.LetWeTrustThese(committee, min_slices, max_slices, min_trust_slices_member, max_trust_slices_member,
                            min_trust_slice_number, max_trust_slice_number,
                            min_slice_redundant, max_slice_redundant);
    //Stellar固定成員,每個節點最少選幾個slice,每個節點最多選幾個slice,每個slice最少相信幾個stellar成員,每個slice最多相信幾個stellar成員,每個slice最少相信幾個其他成員,每個slice最多相信幾個其他成員,

    SimulateSFBA_OnlyGetAcceptQuorum(100, baseCommitteeCnt, SFBA);   //模擬SFBA 100輪
    SimulateStellar(100, Stellar, committee);      //模擬Stellar 100輪
}

