//
// Created by hiccup on 2022/8/5.
//

#pragma once
#include <atomic>
#include <chrono>
// stopwatch. Returns time in seconds
namespace TestingFramework {

    class MyStopWatch {
    public:
        std::chrono::time_point <std::chrono::high_resolution_clock> lastTime;

        MyStopWatch() : lastTime(std::chrono::high_resolution_clock::now()) {}

        inline double elapsed() {
            std::chrono::time_point <std::chrono::high_resolution_clock> thisTime = std::chrono::high_resolution_clock::now();
            double deltaTime = std::chrono::duration<double>(thisTime - lastTime).count();
            lastTime = thisTime;
            return deltaTime;
        }
    };
}