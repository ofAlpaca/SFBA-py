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
        int roundTime;
        std::vector<std::pair<int, int> > gatheredWitness;
    };
    Topology tp;
    std::vector<Node> nodes;
    std::vector<std::vector<info> > Ledger;
    std::vector<info> epoch; //this round
    int nodecnt;
    int blockHeight;
    int currentCommitter;
    int downed;


    Config(Topology &tp, int nodecnt) {
        this->tp = tp;
        this->nodecnt = nodecnt;
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

    void ToggleRandomNodeDown(int size) {
        std::vector<int> shaker;
        for (int i = 0; i < nodecnt; i++) {
            shaker.push_back(i);
        }
        std::random_shuffle(shaker.begin(), shaker.end());
        this->downed = 0;
        for (int i = 0; i < size; i++) {
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
        std::uniform_int_distribution<> r_Special(0,
                                                  10);
        std::vector<int> others;
        for (int i = 0; i < nodecnt; i++) {
            others.push_back(i);
        }
        std::vector<int> special(watcher);
        for (int it = 0; it < nodecnt; it++) {
            int slices = r_slices(Global::rng.get());
            int trustslices = r_trust_slices(Global::rng.get());
            nodes[it].Slices.resize(slices + trustslices);
            for (int i = 0; i < trustslices; i++) {
                int trust_inside_watcher = r_trust_inside_watcher(Global::rng.get());
                std::random_shuffle(watcher.begin(), watcher.end());
                if (r_Special(Global::rng.get())) {
                    for (int j = 0; j < 2; j++) {
                        nodes[it].Slices[i].members.push_back(special[j]);
                    }
                } else {
                    for (int j = 0; j < trust_inside_watcher; j++) {
                        nodes[it].Slices[i].members.push_back(watcher[j]);
                    }
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


    void RandomizeSlices(unsigned int min_slices, unsigned int max_slices, unsigned int min_slice_member,
                         unsigned int max_slice_member) {
        assert(max_slice_member < nodecnt);
        min_slices = std::min(min_slices, static_cast<unsigned int>(1000));
        max_slices = std::min(max_slices, static_cast<unsigned int>(1000));
        min_slice_member = std::max(min_slice_member, static_cast<unsigned int>(1));
        max_slice_member = std::max(max_slice_member, static_cast<unsigned int>(1));
        std::uniform_int_distribution<> r_slices(min_slices, max_slices);
        std::uniform_int_distribution<> r_slice_member(min_slice_member, max_slice_member);
        std::vector<int> vec;
        std::vector<std::pair<int, std::vector<int>>> sliceSort;
        for (int i = 0; i < nodecnt; i++) {
            vec.push_back(i);
        }
        for (int i = 0; i < nodecnt; i++) {
            int slicescnt = r_slices(Global::rng.get());
            sliceSort.clear();
            for (int j = 0; j < slicescnt; j++) {
                std::random_shuffle(vec.begin(), vec.end());
                int slicemember = r_slice_member(Global::rng.get());
                std::vector<int> current;
                for (int k = 0; current.size() < slicemember; k++) {
                    if (vec[k] == i) {
                        continue;
                    }
                    current.push_back(vec[k]);
                }
                if (current.size() == 0) {
                    assert(true);
                }
                sliceSort.push_back({innerCommunicateTime(current), current});
            }
            std::sort(sliceSort.begin(), sliceSort.end());
            this->nodes[i].Slices.clear();
            for (int j = 0; j < slicescnt; j++) {
                Slice s;
                s.members.swap(sliceSort[j].second);
                this->nodes[i].Slices.push_back(s);
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

    std::vector<int> getRandomCommittee(int n, bool bad = false) {
        std::vector<int> vec;
        std::set<int> se;
        std::uniform_int_distribution<> r_committer(0, this->nodecnt - 1);
        std::vector<int> all;
        for (int i = 0; i < this->nodecnt; i++)
            all.push_back(i);
        std::vector<int> winner;
        std::vector<std::pair<int, std::vector<int>>> loser;
        int best = INT_MAX;
        int round = this->nodecnt * 10;
        if (n == this->nodecnt)round = 1;
        if (bad) {
            round = 1;
        }
        for (int i = 0; i < round; i++) {
            std::random_shuffle(all.begin(), all.end());
            vec.clear();
            for (int j = 0; j < n; j++) {
                vec.push_back(all[j]);
            }
            int now = innerCommunicateTime(vec);
            if (bad) {
                std::vector<int> vec2(vec);
                loser.push_back({now, vec2});
            }
            //std::cout << now << std::endl;
            if (now < best) {
                best = now;
                winner.swap(vec);
            }
        }
        if (!bad)
            return winner;
        else {
            std::sort(loser.begin(), loser.end());
            return loser[loser.size() / 2].second;
        }

    }


    int innerCommunicateTime(std::vector<int> nodes) {
        int ret = 0;
        for (int i = 0; i < nodes.size(); i++) {
            for (int j = 0; j < nodes.size(); j++) {
                if (this->nodes[nodes[i]].Down || this->nodes[nodes[j]].Down)continue;
                if (tp.AdjList[nodes[i]][nodes[j]] == INT_MAX) {
                    assert(false);
                }
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

    int CascadingFailure(std::vector<int> committee, std::vector<int> DownedNode) {
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

        ToggleAllNodeUp();
        for (auto it:DownedNode) {
            this->nodes[it].Down = true;
        }

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

        //preprocess
        int sliceID = 0;
        for (auto idx: remain) {
            for (auto &slice: nodes[idx].Slices) {
                for (auto member: slice.members) {
                    setMap[member].push_back(sliceID);
                }
                Slice.push_back(std::vector<int>(slice.members));
                sliceRemain.push_back(slice.members.size());
                idMap[sliceID++] = idx;
            }
        }

        timestampSlice.resize(sliceID);

        std::vector<int> currentRound, nextRound;
        for (auto it:committee) {
            currentRound.push_back(it);
        }


        while (currentRound.size()) {
            //std::cout << currentRound.size() << std::endl;
            for (auto it:currentRound) {
                for (auto sID:setMap[it]) {
                    sliceRemain[sID]--;
                    timestampSlice[sID] = std::max(this->tp.AdjList[it][idMap[sID]] + timestamp[it],
                                                   timestampSlice[sID]);
                    if (sliceRemain[sID] == 0) {
                        nextRound.push_back(idMap[sID]);
                        if (timestampSlice[sID] < timestamp[idMap[sID]]) {
                            timestamp[idMap[sID]] = timestampSlice[sID];
                            parent[idMap[sID]] = sID;
                        }
                    }
                }
            }
            currentRound.swap(nextRound);
            nextRound.clear();
        }


        //calc the time we gather witness*fraction
        int output = 0;
        for (int i = 0; i < this->nodecnt; i++) {
            if (timestamp[i] == INT_MAX) {
                output++;
            }
        }


        return output;

    }

    info
    StartGatherWitness(std::vector<int> committee, std::vector<int> witness, float fraction) {
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

        //preprocess
        int sliceID = 0;
        for (auto idx: remain) {
            for (auto &slice: nodes[idx].Slices) {
                for (auto member: slice.members) {
                    setMap[member].push_back(sliceID);
                }
                Slice.push_back(std::vector<int>(slice.members));
                sliceRemain.push_back(slice.members.size());
                idMap[sliceID++] = idx;
            }
        }

        timestampSlice.resize(sliceID);

        std::vector<int> currentRound, nextRound;
        for (auto it:committee) {
            currentRound.push_back(it);
        }


        while (currentRound.size()) {
            //std::cout << currentRound.size() << std::endl;
            for (auto it:currentRound) {
                for (auto sID:setMap[it]) {
                    int nodeID = idMap[sID];
                    sliceRemain[sID]--;
                    timestampSlice[sID] = std::max(this->tp.AdjList[it][idMap[sID]] + timestamp[it],
                                                   timestampSlice[sID]);
                    if (sliceRemain[sID] == 0) {
                        nextRound.push_back(idMap[sID]);
                        if (timestampSlice[sID] < timestamp[idMap[sID]]) {
                            timestamp[idMap[sID]] = timestampSlice[sID];
                            parent[idMap[sID]] = sID;
                        }
                    }
                }
            }
            currentRound.swap(nextRound);
            nextRound.clear();
        }


        std::set<int> seWitness;
        for (auto &it:witness)seWitness.insert(it);

        std::vector<int> refCnt;
        refCnt.resize(this->nodecnt);

        for (int i = 0; i < this->nodecnt; i++) {
            int sID = parent[i];
            if (sID == -1)continue;
            for (auto &nID:Slice[sID]) {
                refCnt[nID]++;
            }
        }
        for (int i = 0; i < this->nodecnt; i++) {
            if (refCnt[i] > this->nodes[i].bandWidth) {
                timestamp[i] = (double) timestamp[i] * (refCnt[i] / (double) this->nodes[i].bandWidth);
            }
        }

        //calc the time we gather witness*fraction
        std::vector<std::pair<int, int> > vCalcPri;
        for (int i = 0; i < this->nodecnt; i++) {
            vCalcPri.push_back({timestamp[i], i});
        }

        std::vector<std::pair<int, int>> gatheredWitness;
        std::sort(vCalcPri.begin(), vCalcPri.end());
        int now = 0, maxTime = 0;
        for (auto &it:vCalcPri) {
            if (seWitness.count(it.second)) {
                now++;
                if (now <= witness.size() * fraction) {
                    maxTime = std::max(maxTime, it.first);
                }
                gatheredWitness.push_back({it.second, it.first});
            }
        }

        return {maxTime, gatheredWitness};
    }
};


#endif //SFBA_CONFIG_HPP
