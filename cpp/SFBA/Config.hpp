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
        this->tp.GenerateAllPairShortestPath();
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

    std::vector<int> getRandomNodes(int n) {
        std::vector<int> vec;
        for (int i = 0; i < nodecnt; i++) {
            vec.push_back(i);
        }
        random_shuffle(vec.begin(), vec.end());
        return std::vector<int>(vec.begin(), vec.begin() + n);
    }

    int innerCommunicateTime(std::vector<int> nodes) {
        int ret = 0;
        for (int i = 0; i < nodes.size(); i++) {
            for (int j = 0; j < nodes.size(); j++) {
                ret = std::max(ret, tp.AdjList[nodes[i]][nodes[j]]);
            }
        }
        return ret * 2;
    }

    std::vector<int> StartGatherWatcher(std::vector<int> watcher, std::vector<int> committee) {
        std::unordered_set<int> remain;
        std::unordered_set<int> seCommittee, seWatcher;
        for (auto it:watcher) {
            seWatcher.insert(it);
        }
        for (auto it:committee) {
            if (seWatcher.count(it))
                seWatcher.erase(seWatcher.find((it)));
            seCommittee.insert(it);
        }

        for (int i = 0; i < nodecnt; i++) {
            if (!seCommittee.count(i)) {
                remain.insert(i);
            }
        }
        std::vector<int> updateToCommittee;
        for (int rounds = 0;; rounds++) {
            bool updated = false;
            updateToCommittee.clear();
            //check if we can update
            for (auto nowNodeIndex: remain) {
                for (int j = 0; j < nodes[nowNodeIndex].Slices.size(); j++) {
                    bool sliceOK = true;
                    for (int k = 0; k < nodes[nowNodeIndex].Slices[j].members.size(); k++) {
                        if (!seCommittee.count(nodes[nowNodeIndex].Slices[j].members[k])) {
                            sliceOK = false;
                        }
                    }
                    if (sliceOK) {
                        updateToCommittee.push_back(nowNodeIndex);
                        updated = true;
                        break;
                    }
                }
            }
            for (auto it:updateToCommittee) {
                committee.push_back(it);
                seCommittee.insert(it);
                if (seWatcher.count(it))seWatcher.erase(seWatcher.find(it));
                remain.erase(remain.find(it));
            }
            if (seWatcher.size() == 0)break;
            if (!updated)break;
        }
        if (seWatcher.size()) {
            for (auto it:seWatcher)
                std::cout << it << std::endl;
            std::cout << seWatcher.size() << " " << committee.size() << std::endl;
        }
        return committee;
    }
};


#endif //SFBA_CONFIG_HPP
