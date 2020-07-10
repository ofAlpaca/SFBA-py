#include <bits/stdc++.h>
#include "../Topology.h"
#include "../Config.hpp"

using namespace std;

int nodes = 1000;
int edges = nodes * 2;
const int min_slices = 15;
const int max_slices = 20;
const int min_slice_member = 2;
const int max_slice_member = 7;
const int min_trust_slices_member = 2;
const int max_trust_slices_member = 3;
const int min_trust_slice_number = 2;
const int max_trust_slice_number = 3;
const int min_slice_redundant = 2;
const int max_slice_redundant = max_slices - max_trust_slice_number;
const int baseCommitteeCnt = nodes / 4; //smallest committee
const int witnessSize = nodes / 4; //witness count
const int Timeout = 1000;

const float fraction_corrupted = 0.0;

void SimulateSFBA_OnlyGetAcceptQuorum(int rounds, int committeeSize, std::vector<int> witness, Config &config) {
    cout << "SFBA node:" << nodes << endl;
    long double ret = 0;
    for (int i = 0; i < rounds; i++) {
        config.ToggleAllNodeUp();
        config.ToggleRandomNodeDown(fraction_corrupted);
        auto committee = config.getRandomCommittee();
        cout << committee.size() << endl;
        int innerLatency = config.innerCommunicateTime(committee);
        int outerLatency = config.StartGatherWitness(committee, witness, 0.5);
        if (outerLatency == INT_MAX) {
            i--;
            ret += (long long) Timeout + innerLatency;
            //config.splitSlicesRandom();
        } else {
            ret += (long long) outerLatency + innerLatency;
        }
    }
    cout << fixed << ret / rounds << endl;
}

void SimulateStellar(int rounds, Config &config, std::vector<int> stellarCommittee) {
    cout << "Stellar node:" << nodes << endl;
    long double ret = 0;
    int innerLatency = config.innerCommunicateTime(stellarCommittee);
    for (int i = 0; i < rounds; i++) {
        config.ToggleAllNodeUp();
        config.ToggleRandomNodeDown(fraction_corrupted);
        int outerLatency = config.StartGatherWitness(stellarCommittee, std::vector<int>(), 1 - fraction_corrupted);
        ret += (long long) outerLatency + innerLatency;
    }
    cout << fixed << ret / rounds << endl;
}


int main() {
    Topology tp;
    //tp.FromFile("p2p-Gnutella08.txt", 15, 15);

    tp.Random(nodes, edges, 5, 2);


    Config SFBA(tp), Stellar(tp);

    SFBA.RandomizeSlices(min_slices, max_slices, min_slice_member,
                         max_slice_member); //每個節點最少選幾個slice,每個節點最多選幾個slice,每個slice最少成員,每個slice最多成員

    auto committee = Stellar.getRandomNodes(baseCommitteeCnt);   //stellar committee;

    Stellar.LetWeTrustThese(committee, min_slices, max_slices, min_trust_slices_member, max_trust_slices_member,
                            min_trust_slice_number, max_trust_slice_number,
                            min_slice_redundant, max_slice_redundant);
    //Stellar固定成員,每個節點最少選幾個slice,每個節點最多選幾個slice,每個slice最少相信幾個stellar成員,每個slice最多相信幾個stellar成員,每個slice最少相信幾個其他成員,每個slice最多相信幾個其他成員,


    auto witness = SFBA.getRandomNodes(witnessSize);

    SimulateSFBA_OnlyGetAcceptQuorum(1, baseCommitteeCnt, witness, SFBA);   //模擬SFBA 100輪
    //SimulateStellar(10, Stellar, committee);      //模擬Stellar 100輪
}

