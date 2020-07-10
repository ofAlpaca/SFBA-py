//
// Created by misclicked on 2020/6/21.
//

#ifndef SFBA_NODE_HPP
#define SFBA_NODE_HPP


#include <vector>
#include "Slice.h"

enum NodeStatus {
    Vote, Accept, Commit
};

class Node {
public:
    std::vector<Slice> Slices;
    int inputState;
    int outputState;
    int Down;
    int bandWidth;
    int stake;
    NodeStatus internalState;

    Node() {
        internalState = Vote;
        Down = 0;
        stake = 0;
        bandWidth = INT_MAX;
    }
};


#endif //SFBA_NODE_HPP
