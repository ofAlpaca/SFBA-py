//
// Created by misclicked on 2020/7/9.
//

#ifndef SFBA_STELLAR_HPP
#define SFBA_STELLAR_HPP

#include "Config.hpp"

class Stellar : public Config {
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
    int Timeout = 3000;

    std::vector<int> witness;

    int round_per_epoch = 10;

    float fraction_corrupted = 0.0;

    double confTimeSum = 0;
    double confTimeCnt = 0;


    double getAvaCFT() {
        return confTimeSum / confTimeCnt;
    }

    std::vector<double> thresholdDeceive, thresholdBlocking;
    std::vector<std::vector<double>> honestNow, honestPre;
    std::vector<double> availableNow, availablePre;


    double witnessFraction = 0.5;

    Stellar(Topology tp, int nodecnt) :
            Config(tp, nodecnt) {
    };

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
    };

    void Rearrenge() {
        confTimeSum += this->Timeout;
        confTimeCnt++;
        min_slices *= 1.1;
        max_slices *= 1.1;
        min_slice_member *= 0.9;
        max_slice_member *= 0.9;
        RandomizeSlices(min_slices, max_slices, min_slice_member, max_slice_member);
    }

    void Run(int epoch) override {
        confTimeCnt = 0;
        confTimeSum = 0;
        std::vector<int> nowWitness;

        for (int __i = 0; __i < epoch; __i++) {
            this->epoch.clear();
            info rearrengeRound;
            int innerTime;
            std::vector<int> committee;
            for (int __i_ = 0; __i_ < round_per_epoch; __i_++, this->blockHeight++) {
                committee = getRandomCommittee((double) this->nodecnt * (2 / 3.0), true);
                innerTime = innerCommunicateTime(committee);
                confTimeSum += innerTime;
                confTimeCnt++;
            }
            Ledger.push_back(this->epoch);
        }
    };
};

#endif //SFBA_SFBA_HPP
