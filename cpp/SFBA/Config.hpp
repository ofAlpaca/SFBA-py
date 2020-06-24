//
// Created by misclicked on 2020/6/22.
//

#ifndef SFBA_CONFIG_HPP
#define SFBA_CONFIG_HPP


#include "Topology.h"
#include "Node.h"
#include "Global.h"

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

    void
    LetWeTrustThese(std::vector<int> watcher, int min_slices, int max_slices, int min_trust_inside_watcher,
                    int max_trust_inside_watcher, int min_slice_redundant, int max_slice_redundant) {
        assert(watcher.size() >= max_trust_inside_watcher);
        assert(nodecnt - watcher.size() >= max_slice_redundant);
        std::uniform_int_distribution<> r_slices(min_slices, max_slices);
        std::uniform_int_distribution<> r_trust_inside_watcher(min_trust_inside_watcher, max_trust_inside_watcher);
        std::uniform_int_distribution<> r_slice_redundant(min_slice_redundant, max_slice_redundant);
        std::unordered_set<int> seWatcher;
        std::vector<int> others;
        for (auto it:watcher)
            seWatcher.insert(it);
        for (int i = 0; i < nodecnt; i++) {
            if (!seWatcher.count(i))
                others.push_back(i);
        }
        for (auto it:others) {
            int slices = r_slices(Global::rng.get());
            nodes[it].Slices.resize(slices);
            for (int i = 0; i > slices; i++) {
                int trust_inside_watcher = r_trust_inside_watcher(Global::rng.get());
                int slice_redundant = r_slice_redundant(Global::rng.get());
                std::random_shuffle(watcher.begin(), watcher.end());
                std::random_shuffle(others.begin(), others.end());
                for (int j = 0; j < trust_inside_watcher; j++) {
                    nodes[it].Slices[i].members.push_back(watcher[j]);
                }
                for (int j = 0; j < slice_redundant; j++) {
                    if (watcher[j] == it)nodes[it].Slices[i].members.push_back(others.back());
                    else nodes[it].Slices[i].members.push_back(others[j]);
                }
            }

        }
    }

    void RandomizeSlices(int min_slices, int max_slices, int min_slice_member, int max_slice_member) {
        assert(max_slice_member < nodecnt);
        std::uniform_int_distribution<> r_slices(min_slices, max_slices);
        std::uniform_int_distribution<> r_slice_member(min_slice_member, max_slice_member);
        std::vector<int> vec;
        for (int i = 0; i < nodecnt; i++) {
            vec.push_back(i);
        }
        for (int i = 0; i < nodecnt; i++) {
            int slicescnt = r_slices(Global::rng.get());
            nodes[i].Slices.resize(slicescnt);
            for (int j = 0; j < slicescnt; j++) {
                int slicecnt = r_slice_member(Global::rng.get());
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

    int StartGatherWatcher(std::vector<int> watcher, std::vector<int> committee) {
        std::unordered_set<int> remain;
        std::unordered_set<int> seCommittee, seWatcher;
        int *reachedTime = new int[nodecnt];

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
                reachedTime[i] = INT_MAX;
            } else {
                reachedTime[i] = 0;
            }
        }

        std::vector<int> updateToCommittee;
        int virtual_time = 0;
        for (;; virtual_time++) {
            bool updated = false;
            updateToCommittee.clear();
            //check if we can update
            for (auto nowNodeIndex: remain) {
                int sliceAcceptTime = INT_MAX;
                for (int j = 0; j < nodes[nowNodeIndex].Slices.size(); j++) {
                    bool sliceOK = true;
                    int currentSliceAcceptTime = 0;
                    for (int k = 0; k < nodes[nowNodeIndex].Slices[j].members.size(); k++) {
                        if (!seCommittee.count(nodes[nowNodeIndex].Slices[j].members[k])) {
                            sliceOK = false;
                        } else {
                            currentSliceAcceptTime = std::max(currentSliceAcceptTime,
                                                              tp.AdjList[nowNodeIndex][nodes[nowNodeIndex].Slices[j].members[k]]);
                        }
                    }
                    if (sliceOK) {
                        sliceAcceptTime = std::min(sliceAcceptTime, currentSliceAcceptTime);
                        updated = true;
                    }
                }
                if (sliceAcceptTime <= virtual_time) {
                    updateToCommittee.push_back(nowNodeIndex);
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
            virtual_time = INT_MAX;

        }
        std::cout<<"size:"<<committee.size()<<std::endl;
        return virtual_time;
    }
};


#endif //SFBA_CONFIG_HPP
