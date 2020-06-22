#include <bits/stdc++.h>
#include "Topology.h"
#include "Node.h"
#include "Config.hpp"

using namespace std;

int main() {
    Topology tp;
    tp.Random(1000, 8000);

    Config global_state(tp);

    global_state.RandomizeSlices(30, 60, 20, 60);

    return 0;
}

