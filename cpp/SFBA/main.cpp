#include <bits/stdc++.h>
#include "Topology.h"
#include "Config.hpp"

using namespace std;

int main() {
    Topology tp;
    tp.Random(200, 800);

    Config global_state(tp);

    global_state.RandomizeSlices(30, 60, 3, 7);

    auto committee = global_state.getRandomNodes(30);
    auto watcher = global_state.getRandomNodes(100);

    auto final_committee = global_state.StartGatherWatcher(watcher, committee);

    cout << final_committee.size() << endl;

    return 0;
}

