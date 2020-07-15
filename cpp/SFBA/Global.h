//
// Created by misclicked on 2020/6/23.
//

#ifndef SFBA_GLOBAL_HPP
#define SFBA_GLOBAL_HPP


#include <random>
#include <chrono>

class Rng {
public:
    std::mt19937 gen;

    Rng() {
        gen = std::mt19937(69420);
    }

    std::mt19937 &get() {
        return gen;
    }

};

class Global {
public:
    static Rng rng;
};


#endif //SFBA_GLOBAL_HPP
