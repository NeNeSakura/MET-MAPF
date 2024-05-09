//
// Created by hiccup on 2020/5/4.
//

#pragma once

#include <random>

#include <indicators/block_progress_bar.hpp>

namespace TestingFramework {

    class RandomHelper {
    private:
        static std::mt19937 generator;
        static std::mt19937 gene_core;
        static std::uniform_int_distribution<uint32_t> distribution;
        static std::uniform_int_distribution<int> distribution_int;
    public :
        static void InitSeed();

        static uint32_t GetSeed() {
            return distribution(generator);
        };

        static int GetRandomInt() {
            return distribution_int(generator);
        }

        static std::mt19937 &GetCoreGenerator() {
            gene_core = std::mt19937(GetSeed());
            return gene_core;
        }

        static std::mt19937 &GetCoreGenerator(int seed) {
            gene_core = std::mt19937(seed);
            return gene_core;
        }


    };

    using namespace indicators;

    class ProgressBarHelper {
        float progress = 0.0f;
        unsigned total_num = 0;
        float step = 0;
    public:

        size_t current_epoch;
        size_t  total_epoch;
        std::string extra_paint_info;


        BlockProgressBar bar{
                option::BarWidth{80},
                option::Start{"["},
                option::End{"]"},
                option::ForegroundColor{Color::white},
                option::FontStyles{std::vector<FontStyle>{FontStyle::bold}},
                option::ShowElapsedTime{true},
                option::ShowRemainingTime{true}
        };

        void setTotalNum(unsigned n);

        void updateProgressBar();

    static ProgressBarHelper single_instance;

        void showProgressBar();
    };


}



