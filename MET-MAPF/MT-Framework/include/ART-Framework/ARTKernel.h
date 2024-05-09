//
// Created by hiccup on 2021/5/31.
//

#pragma once

#include <vector>
#include <string>
#include <list>
#include <functional>

#include "./Param/KernelParam.hpp"
#include "../MetamorphicTestingKernel.h"
#include <memory>

//#include "Param/TestingConfigTree.h"
//#include "Param/MT_Parameter.hpp"
//#include "Param/MR_Parameter.hpp"
//#include "MAPF/Execution/MAPF_ExecutionBase.h"
//#include "MAPF/Execution/MAPF_Execution.h"

//MAPF_Execution 为临时挂上的啊。
//最终是要写个壳子吧TestingFrame初始化的。

namespace ARTTestingFramework {

    ///using part
    using std::vector;
    using std::string;
    using std::shared_ptr;
    using namespace TestingFramework;


    class ARTKernel {
    public:
        ARTKernel(const ARTKernelParam param, const MR_Parameter &mr_param):param(param),mr_param(mr_param)
        {
//            auto j = json::parse(mr_param.MR_para_str);
//            candidate_num = param.rolling_candidates_num;
            measure_data.reserve(mr_param.testunit_seeds.size());

            relation_ptr = TestingFramework::MetamorphicTestingKernel::relationGeneratorFunc(this->mr_param);
            ART_MT_param = this->param.getMTParam();

        }
        MR_TestUnit_ptr generateNextTestingUnit(MR_TestUnit_ptr origin_candidiate){
            //TODO weak point, prepare to fix?
            //get candidates
            using boost::posix_time::ptime;
            using boost::posix_time::microsec_clock;

            //init
            vector<MeasureItemData> candidates_measure_data;
            candidates_measure_data.reserve(param.rolling_candidates_num + 1);

            //read json
            json o_arg = json::parse(origin_candidiate->origin_case->input_args);
            auto map_name = o_arg["origin_map_name"].get<std::string>();

            //create random scenarios
            vector<MR_TestUnit_ptr> candidates = generateCandidates(map_name,origin_candidiate->follow_cases_list.size());
            candidates.push_back(origin_candidiate);
            //time start
            auto case_start_time = ptime(microsec_clock::universal_time());
            for(const MR_TestUnit_ptr &candidate : candidates){
                candidates_measure_data.push_back(MeasureItemData(candidate, 0,map_name));
            }

            //cal candidates(get min value)
//            checkMeasureData(origin_candidiate);
            checkMeasureData(map_name);
            for (auto &candidate : candidates_measure_data)
                candidate.measure_value = param.measure_ptr->GetMeasureValue(measure_data, candidate.MT_itemData);

            //select max one
            auto candidate_iter = std::max_element(candidates_measure_data.begin(), candidates_measure_data.end(),
                                                   [&](MeasureItemData &lhs, MeasureItemData &rhs){
                                                        return (lhs.measure_value < rhs.measure_value);
                                                   });

            auto case_total_end_time = ptime(microsec_clock::universal_time());

            json o = json::parse( candidate_iter->origin_data_ptr->origin_case->input_args);
            o["ART_genterate_case_microseconds_period"] = (case_total_end_time - case_start_time).total_microseconds();
            candidate_iter->origin_data_ptr->origin_case->input_args = o.dump(2);

            //save to measure data.
            return candidate_iter->origin_data_ptr;
        };

        //update measure data
        void IsTestUnitSuccess(bool success, MR_TestUnit_ptr candidate_iter)
        {
            if(success)
                measure_data.push_back(MeasureItemData(candidate_iter, measure_data.size()));
        }

    private:

        vector<MR_TestUnit_ptr> generateCandidates(string &map_str, const size_t follow_case_num)
        {
            vector<MR_TestUnit_ptr> result;
            //生成 一些新的origin case? 随时准备替换？
//            auto mr_param = mr_group->relation_param;

//            json o_arg = json::parse(test_unit->origin_case->input_args);
//            auto map_name = o_arg["origin_map_name"].get<std::string>();

            mr_param.testunit_seeds.clear();
            for (size_t i = 0; i < param.rolling_candidates_num; i++)
                mr_param.testunit_seeds.push_back(TestingFramework::RandomHelper::GetRandomInt());


            vector<string> origin_params = relation_ptr->originCaseParamGenerator(mr_param, ART_MT_param, map_str);
            result.reserve(origin_params.size()+1); //后面要继续添加一个case
            std::mutex m;
//            for(auto &origin_case_param : origin_params)
            std::for_each(std::begin(origin_params),std::end(origin_params),[&](string origin_case_param)
            {
                auto p = std::allocate_shared<MR_TestUnit>(PoolAlloc::single_instance.testunit_pool_alloc,
                                                           follow_case_num,
                                                           relation_ptr);
                p->generateTestUnit(origin_case_param, mr_param, ART_MT_param);
                std::lock_guard<std::mutex> lock{m};
                result.push_back(p);
            });


            return result;
        }


//        //origin version?
//        vector<MR_TestUnit_ptr> generateCandidates(MR_TestUnit_ptr test_unit)
//        {
//            vector<MR_TestUnit_ptr> result;
//            //生成 一些新的origin case? 随时准备替换？
////            auto mr_param = mr_group->relation_param;
//
//
//            json o_arg = json::parse(test_unit->origin_case->input_args);
//            auto map_name = o_arg["origin_map_name"].get<std::string>();
////            auto agent_num = o_arg["agent_num"].get<ulong>();
////            auto algorithm = o_arg["algorithm"].get<std::string>();
//            mr_param = o_arg["mr_param"].get<MR_Parameter>();
//            mr_param.testunit_seeds.clear();
//            for (size_t i = 0; i < param.rolling_candidates_num; i++)
//                mr_param.testunit_seeds.push_back(TestingFramework::RandomHelper::GetRandomInt());
//
//
//            MT_Relation_ptr relation_ptr = TestingFramework::MetamorphicTestingKernel::relationGeneratorFunc(mr_param);
//            auto ART_MT_param = param.getMTParam();
////            ART_MT_param.MR_list.clear();
////            ART_MT_param.cases_list.clear();
////            ART_MT_param.MR_list.push_back(mr_param);
////            ART_MT_param.cases_list.push_back(map_name);
//
//
//
//            vector<string> origin_params = relation_ptr->originCaseParamGenerator(mr_param, ART_MT_param, map_name);
////            json o_arg = json::parse(test_unit->origin_case->input_args);
////            auto map_name = o_arg["origin_map_name"].get<std::string>();
//            result.reserve(origin_params.size()+1); //后面要继续添加一个case
//            for(auto &origin_case_param : origin_params)
//            {
//                auto p = std::allocate_shared<MR_TestUnit>(PoolAlloc::single_instance.testunit_pool_alloc,
//                                                           test_unit->follow_cases_list.size(),
//                                                           relation_ptr);
//                p->generateTestUnit(origin_case_param, mr_param, ART_MT_param);
//                result.push_back(p);
//            }
//            return result;
//        }

        void checkMeasureData(MR_TestUnit_ptr candidate_iter)
        {
            if(measure_data.size() == 0)
                return;

            json o_arg_1 = json::parse(candidate_iter->origin_case->input_args);
            auto map_name_lhs = o_arg_1["origin_map_name"].get<std::string>();

            if(map_name_lhs != measure_data[0].map_name)
                measure_data.clear();
        }

        void checkMeasureData(const std::string &map_name)
        {
            if(measure_data.size() == 0)
                return;

            if(map_name != measure_data[0].map_name)
                measure_data.clear();
        }


    public:
        ARTKernelParam param;
        std::vector<MeasureItemData>  measure_data;
    private:
//        size_t candidate_num;
        MR_Parameter mr_param;

        MT_Relation_ptr relation_ptr;
        MT_Parameter ART_MT_param;
    };


}




/*
//
// Created by hiccup on 2021/5/31.
//

#pragma once


#include "../include/ARTKernel.h"
#include <algorithm>    // std::shuffle
#include <exception>

//#include <boost/range/irange.hpp>
//#include <boost/range/algorithm_ext/push_back.hpp>


namespace ARTTestingFramework {

//    ARTKernel::ARTKernel(const ARTKernelParam &param, const MeasureData &meatureData)
//            : param(param), datas(meatureData) {
//
//    }
    ARTKernel::ARTKernel(const ARTKernelParam &param, const TestingFramework::MR_ConfigTree &origin_data)
            : param(param), origin_data(origin_data), datas(origin_data) {
    }



    void ARTKernel::execution() {

        //main process
        //step 0 check params
        if (!checkParam())
            throw std::range_error("param num is not fit!");

        //step 1 prepare generated candidates pool
        std::vector<long> idxs;
        for (long idx = 0; idx < datas.input_data.size(); idx++)
            idxs.push_back(idx);
        std::shuffle(idxs.begin(), idxs.end(), param.random_generator);

        auto &output_data = datas.output_data;
        auto &input_data = datas.input_data;
        auto &measure_data = datas.measure_data;
//        output_data.resize(param.iteration_num);


        //step 2 init the meature data
        auto idx = *idxs.rbegin();
        measure_data.push_back(MeasureItemData(input_data[idx], output_data.size()));
        output_data.push_back(input_data[idx]);
        idxs.pop_back();


        //step 3 for loop
        while (output_data.size() < param.iteration_num) {
            std::vector<MeasureItemData> candidates_measure_data;

            //get candidates
            for (auto i = 0; i < param.rolling_candidates_num; i++) {
                auto idx = *idxs.rbegin();
                candidates_measure_data.push_back(MeasureItemData(input_data[idx], output_data.size()));
                idxs.pop_back();
            }

            //cal candidates(get min value)
            for (auto &candidate : candidates_measure_data)
                candidate.measure_value = param.measure.GetMeasureValue(measure_data, candidate.MT_itemData);

            //select max one
            auto candidate_iter = std::max_element(candidates_measure_data.begin(), candidates_measure_data.end(),
                                                   [&](MeasureItemData &lhs, MeasureItemData &rhs){
                                          return (lhs.measure_value < rhs.measure_value);
                                      });

            //save to output data.
            measure_data.push_back(*candidate_iter);
            output_data.push_back(candidate_iter->origin_data_ptr);
        }

    }

    bool ARTKernel::checkParam() {
        if ((param.iteration_num -1) * param.rolling_candidates_num + 1 > datas.input_data.size())
            return false;
        else
            return true;
    }

    void ARTKernel::resultUpdating() {
        if(origin_data.group_list.size() !=1)
            throw std::range_error("origin_data.group_list.size() should be 1!");
        for(auto &g : origin_data.group_list)
        {
            g->unit_list = datas.output_data;
        }

    }

}




 */

