//
// Created by hiccup on 2021/5/31.
//

#pragma once


#include <random>
#include <vector>
#include "../Measurement/IMeasure.hpp"


namespace ARTTestingFramework {


    using std::vector;
    using namespace TestingFramework;

    struct ARTKernelParam {
    public:
        ARTKernelParam(uint rolling_candidates_num, shared_ptr<IMeasure> measure_ptr, MT_Parameter mt_param) :
                rolling_candidates_num(rolling_candidates_num), measure_ptr(measure_ptr),
                rolling_candidates_MT_param(mt_param) {
            RandomHelper::InitSeed();
            seed = RandomHelper::GetSeed();
            random_generator = RandomHelper::GetCoreGenerator(seed);
//            rolling_candidates_MT_param.repeat_num = rolling_candidates_num;
        }

        ARTKernelParam(uint rolling_candidates_num,
                       shared_ptr<IMeasure> measure_ptr,
                       uint _seed, MT_Parameter mt_param) :
                rolling_candidates_num(rolling_candidates_num), measure_ptr(measure_ptr),
                rolling_candidates_MT_param(mt_param) {
            RandomHelper::InitSeed();
            seed = _seed;
            random_generator = RandomHelper::GetCoreGenerator(seed);
//            rolling_candidates_MT_param.repeat_num = rolling_candidates_num;
        }

        const MT_Parameter &getMTParam() {
            return rolling_candidates_MT_param;
        }


    public:
        uint rolling_candidates_num;    //set how many candidates are selected in one turn
//        uint iteration_num;             //set how many candidates are prepared to be selected;
        shared_ptr<IMeasure> measure_ptr;               //set the measure prepared for the ART.
        std::mt19937 random_generator;

    private:
        uint seed;
        MT_Parameter rolling_candidates_MT_param;
    };
}