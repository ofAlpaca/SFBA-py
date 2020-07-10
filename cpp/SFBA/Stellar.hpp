//
// Created by misclicked on 2020/7/9.
//

#ifndef SFBA_STELLAR_HPP
#define SFBA_STELLAR_HPP

#include "Config.hpp"

class Stellar : public Config {
public:
    Stellar(Topology tp) : Config(tp) {};

    void Bootstrap() override {

    };

    void Run(int round) override {

    };
};

#endif //SFBA_STELLAR_HPP
