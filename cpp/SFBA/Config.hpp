//
// Created by misclicked on 2020/6/22.
//

#ifndef SFBA_CONFIG_HPP
#define SFBA_CONFIG_HPP


#include "Topology.h"
#include "Node.hpp"
#include "Global.h"
#include "Dij.hpp"

class Config {
private:

public:
    struct trade {
        int from, to, amount;
    };
    typedef std::set<trade, decltype([](const trade &a, const trade &b) {
        return std::pair<int, int>(a.from, a.to) < std::pair<int, int>(b.from, b.to);
    })> TradeInfo;
    struct info {
        int roundWinner;
        int roundTime;
        double successFraction;
        TradeInfo tradingInfo;
    };
    Topology tp;
    std::vector<Node> nodes;
    std::vector<std::vector<info> > Ledger;
    std::vector<info> epoch; //this round
    int nodecnt, edgecnt;
    int blockHeight;
    int currentCommitter;
    int downed;


    Config(Topology &tp) {
        this->tp = tp;
        nodecnt = this->tp.nodes;
        edgecnt = this->tp.edges;
        blockHeight = 0;
        nodes.resize(nodecnt);
    }

    virtual void Bootstrap() = 0;

    virtual void Run(int round) = 0;

    void RandomizeNodeBandwidth(int mean_bandwidth, int stddev_bandwidth) {
        std::normal_distribution<> r_bandwidth(mean_bandwidth, stddev_bandwidth);
        for (auto &node: nodes) {
            node.bandWidth = std::max((int) r_bandwidth(Global::rng.get()), 1);
        }
    }

    void RandomizeNodeStake(int mean_stake, int stddev_stake) {
        std::normal_distribution<> r_stake(mean_stake, stddev_stake);
        for (auto &node: nodes) {
            node.stake = std::max((int) r_stake(Global::rng.get()), 0);
        }
    }

    TradeInfo
    GenerateRandomTradingInfo(int mean_trading_count, int stddev_trading_count, int mean_trading_amount,
                              int stddev_trading_amount) {
        std::normal_distribution<> r_trading_count(mean_trading_count, stddev_trading_count);
        std::normal_distribution<> r_trading_amount(mean_trading_amount, stddev_trading_amount);
        std::uniform_int_distribution<int> r_node(0, this->nodecnt - 1);
        int trading_count = std::max((int) r_trading_count(Global::rng.get()), 1);

        TradeInfo set_trade;

        for (int i = 0; i < trading_count; i++) {
            int A = r_node(Global::rng.get());
            int B;
            do {
                B = r_node(Global::rng.get());
            } while (A == B && set_trade.count({A, B}));
            int trading_amount = std::max((int) r_trading_amount(Global::rng.get()), 0);
            set_trade.insert(trade{A, B, trading_amount});
        }

        return set_trade;
    }

    void ToggleRandomNodeDown(float fraction) {
        assert(fraction <= 1.0);
        std::vector<int> shaker;
        for (int i = 0; i < nodecnt; i++) {
            shaker.push_back(i);
        }
        std::random_shuffle(shaker.begin(), shaker.end());
        this->downed = 0;
        for (int i = 0; i < nodecnt * fraction; i++) {
            nodes[shaker[i]].Down = 1;
            this->downed++;
        }
    }

    void ToggleAllNodeUp() {
        for (int i = 0; i < nodecnt; i++) {
            nodes[i].Down = 0;
        }
        this->downed = 0;
    }

    void
    LetWeTrustThese(std::vector<int> watcher, int min_slices, int max_slices, int min_trust_slices,
                    int max_trust_slices, int min_trust_inside_watcher,
                    int max_trust_inside_watcher, int min_slice_redundant, int max_slice_redundant) {
        assert(watcher.size() >= max_trust_inside_watcher);
        assert(nodecnt - watcher.size() >= max_slice_redundant);
        std::uniform_int_distribution<> r_slices(min_slices - min_trust_inside_watcher,
                                                 max_slices - max_trust_inside_watcher);
        std::uniform_int_distribution<> r_trust_slices(min_trust_slices, max_trust_slices);
        std::uniform_int_distribution<> r_trust_inside_watcher(min_trust_inside_watcher, max_trust_inside_watcher);
        std::uniform_int_distribution<> r_slice_redundant(min_slice_redundant, max_slice_redundant);
        std::uniform_int_distribution<> r_slice_others(min_slice_redundant + min_trust_inside_watcher,
                                                       max_slice_redundant + max_trust_inside_watcher);
        std::vector<int> others;
        for (int i = 0; i < nodecnt; i++) {
            others.push_back(i);
        }
        for (int it = 0; it < nodecnt; it++) {
            int slices = r_slices(Global::rng.get());
            int trustslices = r_trust_slices(Global::rng.get());
            nodes[it].Slices.resize(slices + trustslices);
            for (int i = 0; i < trustslices; i++) {
                int trust_inside_watcher = r_trust_inside_watcher(Global::rng.get());
                std::random_shuffle(watcher.begin(), watcher.end());
                for (int j = 0; j < trust_inside_watcher; j++) {
                    nodes[it].Slices[i].members.push_back(watcher[j]);
                }
            }
            for (int i = trustslices; i < trustslices + slices; i++) {
                int slice_redundant = r_slice_others(Global::rng.get());
                std::random_shuffle(others.begin(), others.end());
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

    std::vector<int> getRandomCommittee() {
        std::vector<int> vec;
        std::set<int> se;
        std::uniform_int_distribution<> r_committer(0, this->nodecnt - 1);
        int committer = r_committer(Global::rng.get());
        for (auto &slice: this->nodes[committer].Slices) {
            for (auto &node: slice.members) {
                if (this->nodes[node].Down)continue;
                se.insert(node);
            }
        }
        for (auto it:se) {
            vec.push_back(it);
        }
        currentCommitter = committer;
        return vec;
    }


    int innerCommunicateTime(std::vector<int> nodes) {
        int ret = 0;
        for (int i = 0; i < nodes.size(); i++) {
            for (int j = 0; j < nodes.size(); j++) {
                if (this->nodes[nodes[i]].Down || this->nodes[nodes[j]].Down)continue;
                ret = std::max(ret, tp.AdjList[nodes[i]][nodes[j]]);
            }
        }
        return ret * 2;
    }

    void splitSlicesRandom() {
        for (int i = 0; i < nodecnt; i++) {
            int sz = nodes[i].Slices.size();
            std::uniform_int_distribution<> r_slices(0, sz - 1);
            int id = r_slices(Global::rng.get());
            int sz2 = nodes[i].Slices[id].members.size();
            std::uniform_int_distribution<> r_slices2(0, sz2 - 1);
            int id2 = r_slices2(Global::rng.get());
            std::vector<int> tmpA(nodes[i].Slices[id].members.begin(), nodes[i].Slices[id].members.begin() + id2);
            std::vector<int> tmpB(nodes[i].Slices[id].members.begin() + id2, nodes[i].Slices[id].members.end());
            nodes[i].Slices.erase(nodes[i].Slices.begin() + id);
            Slice A;
            A.members.swap(tmpA);
            Slice B;
            B.members.swap(tmpB);
            if (A.members.size())
                nodes[i].Slices.push_back(A);
            if (B.members.size())
                nodes[i].Slices.push_back(B);
        }
    }

    info
    StartGatherWitness(std::vector<int> committee, std::vector<int> witness, float fraction, int mean_trading_count,
                       int stddev_trading_count, int mean_trading_amount, int stddev_trading_amount) {
        assert(fraction <= 1.0);
        assert(fraction >= 0);
        std::vector<int> tmpCommittee;

        std::vector<int> timestamp;
        std::vector<int> timestampSlice;
        std::vector<int> parent;            //which slice node ref this round
        std::vector<int> penalty;           //penalty when reached node.bandWidth
        std::vector<int> capacity;           //used to calc current bandwidth
        std::vector<int> sliceRemain;
        std::vector<std::vector<int> > Slice;
        std::unordered_set<int> remain;
        std::unordered_set<int> seCommittee;
        std::vector<std::vector<int>> setMap;   //N way list to reduce
        std::unordered_map<int, int> idMap; // transfer slice id to real id

        timestamp.resize(this->nodecnt, INT_MAX);
        penalty.resize(this->nodecnt);
        setMap.resize(this->nodecnt);
        capacity.resize(this->nodecnt);
        parent.resize(this->nodecnt, -1);


        //clear out "down" nodes
        tmpCommittee.swap(committee);
        for (auto it:tmpCommittee)
            if (nodes[it].Down != 1)
                committee.push_back(it);

        //initialize timestamp
        for (auto it:committee)
            timestamp[it] = 0;

        //gather remaining nodes
        for (auto it:committee)
            seCommittee.insert(it);
        for (int i = 0; i < nodecnt; i++)
            if (!seCommittee.count(i))
                if (nodes[i].Down != 1)
                    remain.insert(i);

        std::unordered_set<int> seGatheredWitness;
        std::priority_queue<std::pair<int, int>> pq;
        std::unordered_set<int> seWitness;
        for (auto &it:witness) {
            if (this->nodes[it].Down)continue;
            seWitness.insert(it);
        }
        int maxTime = 0;

        for (int i = 0; i < committee.size(); i++) {
            if (seWitness.count(committee[i]))seGatheredWitness.insert(committee[i]);
            for (int j = 0; j < witness.size(); j++) {
                if (this->nodes[witness[j]].Down)continue;
                pq.push({this->tp.AdjList[committee[i]][witness[j]], witness[j]});
            }
        }

        for (auto it:seGatheredWitness) {
            seWitness.erase(it);
        }
        while (pq.size() && (double) seGatheredWitness.size() < (double) witness.size() * fraction) {
            auto now = pq.top();
            pq.pop();
            if (seGatheredWitness.count(now.second))continue;
            seGatheredWitness.insert(now.second);
            seWitness.erase(now.second);
            maxTime += now.first;
            for (auto it:seWitness) {
                pq.push({this->tp.AdjList[now.second][it], it});
            }
        }

        if ((double) seGatheredWitness.size() < (double) witness.size() * fraction)
            maxTime = INT_MAX;

        return {currentCommitter, maxTime,
                -1,
                GenerateRandomTradingInfo(mean_trading_count, stddev_trading_count, mean_trading_amount,
                                          stddev_trading_amount)};
    }
};


#endif //SFBA_CONFIG_HPP
