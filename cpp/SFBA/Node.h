//
// Created by misclicked on 2020/6/21.
//

#ifndef SFBA_NODE_H
#define SFBA_NODE_H


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
    NodeStatus internalState;

    Node() {
        internalState = Vote;
        Down = 0;
    }
};


#endif //SFBA_NODE_H
