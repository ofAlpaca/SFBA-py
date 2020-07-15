//
// Created by misclicked on 2020/7/9.
//

#ifndef SFBA_SFBA_HPP
#define SFBA_SFBA_HPP

#include "Config.hpp"

class SFBA : public Config {
public:
    //system parameter
    double min_slices = 15;
    double max_slices = 20;
    double min_slice_member = 2;
    double max_slice_member = 5;
    int mean_bandwidth = 15;
    int stddev_bandwidth = 10;
    int mean_trading_count = 1000;
    int stddev_trading_count = 10;
    int mean_trading_amount = 10;
    int stddev_trading_amount = 5;
    int mean_stake = 1000;
    int stddev_stake = 200;
    double mean_failChance = 0.05;
    double stddev_failChance = 0.02;
    int witnessSize; //witness count
    int Timeout = 10000;

    std::vector<int> witness;

    int round_per_epoch = 300;

    float fraction_corrupted = 0.0;

    int corruptedNode = 0;

    double decayRateMalice = 0.9;
    double decayRateCurrentMalice = 1;
    double decayRateSumMalice = 0;

    double decayRateFailure = 0.9;
    double decayRateCurrentFailure = 1;
    double decayRateSumFailure = 0;

    double confTimeSum = 0;
    double confTimeCnt = 0;


    double getAvaCFT() {
        return confTimeSum / confTimeCnt;
    }

    std::vector<double> thresholdDeceive, thresholdBlocking;
    std::vector<std::vector<double>> honestNow, honestPre;
    std::vector<double> availableNow, availablePre;


    double witnessFraction = 0.5;

    SFBA(Topology tp, int nodecnt) : Config(tp, nodecnt) {
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


    void ToggleSpecialNodeDown(float fraction) {
        assert(fraction <= 1.0);
        std::vector<int> shaker;
        for (int i = 0; i < this->witnessSize; i++) {
            shaker.push_back(this->witness[i]);
        }
        std::random_shuffle(shaker.begin(), shaker.end());
        this->downed = 0;
        for (int i = 0; i < nodecnt * fraction; i++) {
            nodes[shaker[i]].Down = 1;
            this->downed++;
        }
    }

    void Bootstrap() override {
        //witnessSize = this->nodecnt * witnessFraction;
        //witnessSize = 10;
        this->min_slices = this->nodecnt * 0.075;
        this->max_slices = this->nodecnt * 0.1;
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
        confTimeSum = 0;
        confTimeCnt = 0;
        RandomizeNodeStake(mean_stake, stddev_stake);
        RandomizeNodeBandwidth(mean_bandwidth, stddev_bandwidth);
        RandomizeSlices(min_slices, max_slices, min_slice_member, max_slice_member);
    };
    double slicesBuff = 1.0;
    double slice_memberBuff = 1.0;

    void Rearrenge() {
        if (confTimeSum + this->Timeout < 0) {
            assert(false);
        }
        confTimeSum += this->Timeout;
        confTimeCnt++;
        std::cerr << this->corruptedNode << std::endl;
        RandomizeSlices(min_slices * slicesBuff, max_slices * slicesBuff, min_slice_member * slice_memberBuff,
                        max_slice_member * slice_memberBuff);
    }

    void Run(int epoch) override {
        confTimeCnt = 0;
        confTimeSum = 0;
        std::vector<int> nowWitness;

        int shouldRun = epoch * round_per_epoch;

        for (int __i = 0; confTimeCnt < shouldRun; __i++) {
            ToggleAllNodeUp();
            ToggleRandomNodeDown(corruptedNode);
            this->epoch.clear();
            info rearrengeRound;
            int innerTime;
            std::vector<int> committee;
            slicesBuff = 1.0;
            slice_memberBuff = 1.0;
            while (true) {
                witness = getRandomNodes(witnessSize);
                committee = getRandomCommittee(25);
                innerTime = innerCommunicateTime(committee);
                //auto committee2 = getRandomCommittee(30, true);
                //int innerTime2 = innerCommunicateTime(committee2);
                rearrengeRound = StartGatherWitness(committee, witness, 0.5);
                if (rearrengeRound.roundTime <= 66666) {
                    break;
                } else {
                    slicesBuff *= 1.05;
                    slice_memberBuff *= 0.95;
                    Rearrenge();
                }
            }
            rearrengeRound.roundTime += innerTime;
            if (confTimeSum + rearrengeRound.roundTime < 0) {
                assert(false);
            }
            confTimeSum += rearrengeRound.roundTime;
            confTimeCnt++;
            nowWitness.clear();
            for (int i = 0; i < rearrengeRound.gatheredWitness.size(); i++) {
                nowWitness.push_back(rearrengeRound.gatheredWitness[i].first);
            }
            for (int __i_ = 0; __i_ < round_per_epoch; __i_++, this->blockHeight++) {
                ToggleAllNodeUp();
                ToggleRandomNodeDown(corruptedNode);
                std::vector<int> tmp;
                tmp.resize(nowWitness.size(), INT_MAX);
                for (auto u:committee) {
                    if (this->nodes[u].Down)continue;
                    for (int i = 0; i < nowWitness.size(); i++) {
                        int v = nowWitness[i];
                        if (this->nodes[v].Down)continue;
                        tmp[i] = std::min(tmp[i], this->tp.AdjList[u][v]);
                    }
                }
                std::sort(tmp.begin(), tmp.end());
                int confirmTime = 0;
                for (int i = 0; i < nowWitness.size() * 0.5; i++) {
                    if (tmp[i] == INT_MAX) {
                        confirmTime = this->Timeout;
                        break;
                    }
                    confirmTime += tmp[i];
                }
                if (confirmTime != this->Timeout) {
                    confirmTime += innerCommunicateTime(committee) / 2;
                    if (confTimeSum + confirmTime < 0) {
                        assert(false);
                    }
                    confTimeSum += confirmTime;
                    confTimeCnt++;
                } else {
                    break;
                }
            }
            Ledger.push_back(this->epoch);
        }
    };
};

#endif //SFBA_SFBA_HPP
