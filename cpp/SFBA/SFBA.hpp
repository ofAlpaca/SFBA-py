//
// Created by misclicked on 2020/7/9.
//

#ifndef SFBA_SFBA_HPP
#define SFBA_SFBA_HPP

#include "Config.hpp"

class SFBA : public Config {
public:
    //system parameter
    int min_slices = 2;
    int max_slices = 7;
    int min_slice_member = 2;
    int max_slice_member = 7;
    int mean_bandwidth = 12;
    int stddev_bandwidth = 1;
    int mean_trading_count = 100;
    int stddev_trading_count = 10;
    int mean_trading_amount = 10;
    int stddev_trading_amount = 5;
    int mean_stake = 1000;
    int stddev_stake = 200;
    double mean_failChance = 0.05;
    double stddev_failChance = 0.02;
    int witnessSize; //witness count
    int Timeout = 1000;

    std::vector<int> witness;

    int round_per_epoch = 200;

    float fraction_corrupted = 0.0;

    double decayRateMalice = 0.9;
    double decayRateCurrentMalice = 1;
    double decayRateSumMalice = 0;

    double decayRateFailure = 0.9;
    double decayRateCurrentFailure = 1;
    double decayRateSumFailure = 0;


    std::vector<double> thresholdDeceive, thresholdBlocking;
    std::vector<std::vector<double>> honestNow, honestPre;
    std::vector<double> availableNow, availablePre;


    SFBA(Topology tp) : Config(tp) {
        witnessSize = this->nodecnt / 4;
    };

    double getNodeMalice(int from, int to) {
        if (this->blockHeight == 1)return 0;
        return 1 - honestNow[from][to] / decayRateSumMalice;
    }

    double getNodeBandwidth(int id) {
        return this->nodes[id].bandWidth;
    }

    double getNodeFailChance(int id) {
        if (this->blockHeight == 1)return 0;
        return 1 - availableNow[id] / decayRateSumMalice;
    }

    int getNodeStake(int id) {
        return this->nodes[id].stake;
    }

    void
    RandomizeSlicesInFlavorOfMaliceAndBandwidth(int min_slices, int max_slices, int min_slice_member,
                                                int max_slice_member) {
        std::uniform_int_distribution<> r_slices(min_slices, max_slices);
        std::uniform_int_distribution<> r_slices_member(min_slice_member, max_slice_member);
        std::uniform_int_distribution<> r_node(0, this->nodecnt - 1);
        std::vector<int> vec;
        for (int i = 0; i < nodecnt; i++) {
            vec.push_back(i);
        }
        for (int i = 0; i < nodecnt; i++) {
            //nodes[i].Slices.resize(slicescnt);
            std::vector<std::vector<int>> _slices;
            std::vector<double> cost, value;
            std::vector<std::pair<double, int>> sliceSort;
            for (int j = 0; j < max_slices * 10; j++) {
                int slicesmembercnt = r_slices_member(Global::rng.get());
                double deceiveProb = 1;
                double bandWidthSum = 0;
                std::vector<int> slice;
                for (int k = 0; k < slicesmembercnt; k++) {
                    int memberID;
                    do {
                        memberID = r_node(Global::rng.get());
                    } while (memberID == i);
                    slice.push_back(memberID);
                    bandWidthSum += getNodeBandwidth(memberID);
                    deceiveProb *= getNodeMalice(i, memberID);
                    if (deceiveProb <= thresholdDeceive[i])break;
                }
                //cost.push_back(deceiveProb);
                value.push_back(bandWidthSum);
                _slices.push_back(slice);
            }
            std::unordered_set<int> blockingSet;
            for (int j = 0; j < _slices.size(); j++) {
                sliceSort.push_back({value[j], j});
            }
            sort(sliceSort.begin(), sliceSort.end(), std::greater<std::pair<double, int>>());
            int slicescnt = r_slices(Global::rng.get());
            int sliceCounter = 0;

            for (int j = 0; j < slicescnt; j++, sliceCounter++) {
                int nowSliceID = sliceSort[sliceCounter].second;
                std::unordered_set<int> tmp(blockingSet);
                if (tmp.size() == 0) {
                    for (auto &it:_slices[nowSliceID]) {
                        tmp.insert(it);
                    }
                } else {
                    for (auto &it:_slices[nowSliceID]) {
                        tmp.erase(it);
                    }
                }
                //calculate blocking chance
                double nonBlockingChance = 1;
                for (auto &it:tmp) {
                    nonBlockingChance *= 1 - getNodeFailChance(it);
                }
                if ((1 - nonBlockingChance) <= thresholdBlocking[i]) {
                    blockingSet.swap(tmp);
                    Slice now;
                    now.members.swap(_slices[nowSliceID]);
                    this->nodes[i].Slices.push_back(now);
                }
            }
            //std::cout << this->nodes[i].Slices.size() << " " << slicescnt << std::endl;
        }
    }

    void updateProbOfMalice(TradeInfo &tradingInfo) {
        //calc everyone's stake proportion
        long long int stakeMax(0);
        for (int i = 0; i < this->nodecnt; i++) {
            stakeMax = std::max((long long int) getNodeStake(i), stakeMax);
        }
        std::vector<double> stakePorportion;
        stakePorportion.resize(this->nodecnt, 0.0);
        for (int i = 0; i < this->nodecnt; i++) {
            stakePorportion[i] = (double) getNodeStake(i) / stakeMax;
        }

        honestPre.swap(honestNow);
        for (int i = 0; i < this->nodecnt; i++) {
            for (int j = 0; j < this->nodecnt; j++) {
                if (this->blockHeight == 1)
                    honestNow[i][j] = decayRateMalice * (false ? (0 + stakePorportion[j]) : 1) * honestPre[i][j] + 1;
                else
                    honestNow[i][j] =
                            decayRateMalice * (false ? (0 + stakePorportion[j]) : 1) * honestPre[i][j] +
                            ((tradingInfo.count({i, j})) || (tradingInfo.count({j, i})) ? 1.0 : 0.0);
            }
        }
        decayRateSumMalice += decayRateCurrentMalice;
        decayRateCurrentMalice *= decayRateMalice;
    }

    void updateProbOfFaliure(TradeInfo &tradingInfo) {
        //calc everyone's bandWidth proportion
        long long int bandWidthMax(0);
        for (int i = 0; i < this->nodecnt; i++) {
            bandWidthMax = std::max((long long int) getNodeBandwidth(i), bandWidthMax);
        }
        std::vector<double> bandWidthPorportion;
        bandWidthPorportion.resize(this->nodecnt, 0.0);
        for (int i = 0; i < this->nodecnt; i++) {
            bandWidthPorportion[i] = getNodeBandwidth(i) / bandWidthMax;
        }

        //calc who's currently in round]
        std::set<int> currentInRound;
        for (auto &it:tradingInfo) {
            currentInRound.insert(it.from);
        }

        availablePre.swap(availableNow);
        for (int i = 0; i < this->nodecnt; i++) {
            if (this->blockHeight == 1)
                availableNow[i] = decayRateFailure * (false ? bandWidthPorportion[i] : 1) * availablePre[i] +
                                  1;
            else
                availableNow[i] = decayRateFailure * (false ? bandWidthPorportion[i] : 1) * availablePre[i] +
                                  (currentInRound.count(i)) ? 1.0 : 0.0;
        }
        decayRateSumFailure += decayRateCurrentFailure;
        decayRateCurrentFailure *= decayRateFailure;
    }

    void updateSlices() {
        for (int i = 0; i < this->nodecnt; i++) {
            std::vector<Slice> nextSlice, nextSlice2;
            for (auto &slice: this->nodes[i].Slices) {
                double probOfNotDeceive = 1;
                for (auto member: slice.members) {
                    double maliceprob = getNodeMalice(i, member);
                    //std::cout << "malice: " << maliceprob << std::endl;
                    probOfNotDeceive *= 1 - getNodeMalice(i, member);
                }
                if (1 - probOfNotDeceive <= thresholdDeceive[i]) {
                    nextSlice.push_back(slice);
                }
            }
            std::unordered_set<int> blockingSet;
            for (int j = 0; j < nextSlice.size(); j++) {
                std::unordered_set<int> tmp(blockingSet);
                if (tmp.size() == 0) {
                    for (auto it:nextSlice[j].members)tmp.insert(it);
                } else {
                    for (auto it:nextSlice[j].members)tmp.erase(it);
                }
                double probOfNotBlock = 1;
                for (auto it:tmp) {
                    double failchance = getNodeFailChance(it);
                    //std::cout << "fail: " << failchance << std::endl;
                    probOfNotBlock *= 1 - getNodeFailChance(it);
                }
                if (1 - probOfNotBlock <= thresholdBlocking[i]) {
                    blockingSet.swap(tmp);
                    nextSlice2.push_back(nextSlice[j]);
                }
            }
            this->nodes[i].Slices.swap(nextSlice2);
        }
    }

    void releaseThreshold(double releaseRateDeceive = 2.0, double releaseRateBlocking = 2.0) {
        for (int i = 0; i < this->nodecnt; i++) {
            thresholdDeceive[i] = std::min(thresholdDeceive[i] * releaseRateDeceive, 1.1);
            thresholdBlocking[i] = std::min(thresholdBlocking[i] * releaseRateBlocking, 1.1);
        }
    }

    void Bootstrap() override {
        this->blockHeight = 1;
        honestNow.resize(nodecnt);
        for (auto &it:honestNow)it.resize(nodecnt, 0);
        honestPre.resize(nodecnt);
        for (auto &it:honestPre)it.resize(nodecnt);
        availableNow.resize(nodecnt, 0);
        availablePre.resize(nodecnt);
        thresholdDeceive.resize(this->nodecnt, 0.6);   // init x: can afford x% of chance being blocked
        thresholdBlocking.resize(this->nodecnt, 0.6); // init x: can afford x% of chance being blocked
        witness = getRandomNodes(witnessSize);
        Ledger.clear();
        RandomizeNodeStake(mean_stake, stddev_stake);
        RandomizeNodeBandwidth(mean_bandwidth, stddev_bandwidth);
        RandomizeSlicesInFlavorOfMaliceAndBandwidth(min_slices, max_slices, min_slice_member, max_slice_member);
    };

    void Run(int epoch) override {
        for (int __i = 0; __i < epoch; __i++) {
            this->epoch.clear();
            for (int __i_ = 0; __i_ < round_per_epoch; __i_++, this->blockHeight++) {
                ToggleAllNodeUp();
                ToggleRandomNodeDown(fraction_corrupted);
                auto committee = getRandomCommittee();
                int innerLatency = innerCommunicateTime(committee);
                info currentRound = StartGatherWitness(committee, witness, 0.5, mean_trading_count,
                                                       stddev_trading_count, mean_trading_amount,
                                                       stddev_trading_amount);
                if (currentRound.roundTime == INT_MAX) {
                    currentRound.roundTime = innerLatency + Timeout;
                    currentRound.roundWinner = -1;
                    releaseThreshold();
                    RandomizeSlicesInFlavorOfMaliceAndBandwidth(min_slices, max_slices, min_slice_member,
                                                                max_slice_member);
                }
                std::cout << currentRound.roundTime << "\t" << this->downed << " " << currentRound.successFraction
                          << std::endl;
                updateProbOfMalice(currentRound.tradingInfo);
                updateProbOfFaliure(currentRound.tradingInfo);
                //updateSlices();
                this->epoch.push_back(currentRound);
            }
            Ledger.push_back(this->epoch);
        }
    };
};

#endif //SFBA_SFBA_HPP
