//
// Created by misclicked on 2020/6/21.
//

#ifndef SFBA_NODE_HPP
#define SFBA_NODE_HPP


#include <vector>
#include "Slice.h"


class Node {
public:
    std::vector<Slice> Slices;
    int Down;
    int bandWidth;
    int stake;

    Node() {
        Down = 0;
        stake = 0;
        bandWidth = INT_MAX;
    }
};


#endif //SFBA_NODE_HPP
