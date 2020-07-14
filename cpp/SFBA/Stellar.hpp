//
// Created by misclicked on 2020/7/9.
//

#ifndef SFBA_STELLAR_HPP
#define SFBA_STELLAR_HPP

#include "Config.hpp"

class Stellar : public Config {
public:
    //system parameter
    int min_slices = 8;
    int max_slices = 12;
    int mean_bandwidth = 12;
    int stddev_bandwidth = 1;
    int mean_trading_count = 100;
    int stddev_trading_count = 10;
    int mean_trading_amount = 10;
    int stddev_trading_amount = 5;
    int mean_stake = 1000;
    int stddev_stake = 200;
    int witnessSize; //witness count
    int Timeout = 1000;

    std::vector<int> witness;
    std::vector<int> committee;

    int round_per_epoch = 1000;

    float fraction_corrupted = 0.0;

    int min_trust_slices_member = 1;
    int max_trust_slices_member = 1;
    int min_trust_slice_number = 5;
    int max_trust_slice_number = 7;
    int min_slice_redundant = 2;
    int max_slice_redundant = max_slices - max_trust_slice_number;

    std::vector<double> thresholdDeceive, thresholdBlocking;
    std::vector<std::vector<double>> honestNow, honestPre;
    std::vector<double> availableNow, availablePre;


    Stellar(Topology tp) : Config(tp) {
        witnessSize = this->nodecnt;
        max_slice_redundant = max_slices - max_trust_slice_number;
    };


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
        committee = getRandomNodes(30);
        Ledger.clear();
        RandomizeNodeStake(mean_stake, stddev_stake);
        RandomizeNodeBandwidth(mean_bandwidth, stddev_bandwidth);
        LetWeTrustThese(committee, min_slices, max_slices, min_trust_slices_member, max_trust_slices_member,
                        min_trust_slice_number, max_trust_slice_number,
                        min_slice_redundant, max_slice_redundant);
    };

    void Run(int epoch) override {
        std::normal_distribution<> r_stake(5, 2);
        for (int __i = 0; __i < epoch; __i++) {
            this->epoch.clear();
            for (int __i_ = 0; __i_ < round_per_epoch; __i_++, this->blockHeight++) {
                ToggleAllNodeUp();
                ToggleRandomNodeDown(fraction_corrupted);
                int innerLatency = innerCommunicateTime(committee);
                innerLatency += r_stake(Global::rng.get());
                info currentRound = StartGatherWitness(committee, witness, 0.5, mean_trading_count,
                                                       stddev_trading_count, mean_trading_amount,
                                                       stddev_trading_amount);
                if (currentRound.roundTime == INT_MAX) {
                    currentRound.roundTime = innerLatency + Timeout;
                    currentRound.roundWinner = -1;
                }
                std::cout << this->blockHeight << "\t" << currentRound.roundTime + innerLatency << "\t"
                          << this->nodecnt - this->downed
                          << "\t"
                          << this->nodecnt - this->downed - currentRound.successFraction
                          << std::endl;
                //updateSlices();
                this->epoch.push_back(currentRound);
            }
            Ledger.push_back(this->epoch);
        }
    };
};

#endif //SFBA_STELLAR_HPP
