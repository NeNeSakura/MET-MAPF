//
// Created by hiccup on 2022/8/16.
//

#pragma once

#include <iostream>
#include <random>
#include <string>
#include <vector>
#include "MR_ParameterGenerator.h"



using namespace TestingFramework;

class MR_ParameterGeneratorV2 : public MR_ParameterGenerator {
//private:
//    size_t test_unit_num  = 5 * 20;
//    bool total_random = true;
//    std::vector<unsigned> random_list;
public:

    MR_ParameterGeneratorV2(const size_t test_unit_num, bool total_random = false) :  MR_ParameterGenerator(test_unit_num,total_random)
    {
//        total_random = true;
    }


    MR_ParameterGeneratorV2(const std::vector<unsigned> &random_list) : MR_ParameterGenerator(random_list)
    {
//        this->test_unit_num = random_list.size();
//        total_random = false;
    }

    virtual std::vector<MR_Parameter> getVector() override;

};





