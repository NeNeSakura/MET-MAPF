//
// Created by hiccup on 2020/9/10.
//

#pragma once

#include <iostream>
#include <random>
#include <string>
#include <vector>
#include <include/Param/MR_Parameter.hpp>



using namespace TestingFramework;

class MR_ParameterGenerator {
protected:
    size_t test_unit_num  = 5 * 20;
    bool total_random = true;
    std::vector<unsigned> random_list;

public:

    MR_ParameterGenerator(const size_t test_unit_num, bool is_total_random =false) :  test_unit_num(test_unit_num), total_random(is_total_random)
    {
//        total_random = is_total_random;
    }


    MR_ParameterGenerator(const std::vector<unsigned> &random_list) : random_list(random_list), total_random(false)
    {
        this->test_unit_num = random_list.size();
    }

    void seed_setting( std::vector<MR_Parameter>& param_list);

    virtual std::vector<MR_Parameter> getVector();

    std::vector<MR_Parameter> getRandomVector();
};



