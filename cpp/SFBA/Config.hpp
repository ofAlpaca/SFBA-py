//
// Created by misclicked on 2020/6/22.
//

#ifndef SFBA_CONFIG_HPP
#define SFBA_CONFIG_HPP


#include "Topology.h"
#include "Node.h"

class Config {
private:

public:
    Topology tp;
    std::vector<Node> nodes;
    int nodecnt, edgecnt;

    Config(Topology tp) {
        this->tp = tp;
        nodecnt = this->tp.nodes;
        edgecnt = this->tp.edges;
        nodes.resize(nodecnt);
    }

    void RandomizeSlices(int min_slices, int max_slices, int min_slice_member, int max_slice_member) {
        assert(max_slice_member < nodecnt);
        std::mt19937 gen;
        std::uniform_int_distribution<> r_slices(min_slices, max_slices);
        std::uniform_int_distribution<> r_slice_member(min_slice_member, max_slice_member);
        std::vector<int> vec;
        for (int i = 0; i < nodecnt; i++) {
            vec.push_back(i);
        }
        for (int i = 0; i < nodecnt; i++) {
            int slicescnt = r_slices(gen);
            nodes[i].Slices.resize(slicescnt);
            for (int j = 0; j < slicescnt; j++) {
                int slicecnt = r_slice_member(gen);
                std::random_shuffle(vec.begin(), vec.end());
                for (int k = 0; k < slicecnt; k++) {
                    if (vec[k] == i)nodes[i].Slices[j].members.push_back(vec.back());
                    else nodes[i].Slices[j].members.push_back(vec[k]);
                }
            }
        }
    }

    void find_committee() {

    }
};


#endif //SFBA_CONFIG_HPP
