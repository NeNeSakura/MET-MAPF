//
// Created by hiccup on 2020/5/21.
//

#pragma once

#include "MAPF_RelationBase.h"
#include "../DataHandler/MAPF_AnalysisBase.h"
#include "../Operator/MAPF_ObstacleOperator.h"
#include "../../../../mem_pool/include/ObjPool.hpp"


namespace TestingFramework {
    namespace MAPF {
        class DecreaseObstacle : public MAPF_MetamorphicRelationBase {
        public:

            DecreaseObstacle(const MR_Parameter &mr_param)
                    : MAPF_MetamorphicRelationBase(mr_param,
                                                   ObjPool::allocate<OperDecreaseObstacle>(mr_param.MR_para_str)) {
                //parameter list
                // erosion == 1 ?
                // random == 1 ?
                // ratio == [0,1] ?
                violation_criteria_func = ViolationCriteria::Less<double>;

            };


            bool isRelationSatisfied(const string &origin_result, const vector<string> &follow_results) override {
                RelationAnalyzeInfo info(origin_result, follow_results);

                vector<double> lengths;
                lengths.push_back(RelationAnalyzeInfo::GetLength(info.origin));
                for (auto &tmp : info.follows)
                    lengths.push_back(RelationAnalyzeInfo::GetLength(tmp));

//                return std::all_of(lengths.begin() + 1, lengths.end(),
//                                   [&](auto i) {
//                                       return i <= lengths[0] * (1 + info.threshold);
//                                   });
                return violation_criteria_func(lengths, info.threshold);

            }


        public:

            vector<string>
            followCaseParamGenerator(string &origin_case_param, const string &origin_case_result) override {
                if (operator_ptr == NULL)
                    LOG(ERROR) << "@DecreaseObstacle Class, operator_ptr is empty";

                return MAPF_MetamorphicRelationBase::followCaseParamGenerator(origin_case_param, origin_case_result);
            }

        };

        class IncreaseObstacle : public MAPF_MetamorphicRelationBase {
        public:
            IncreaseObstacle(const MR_Parameter &mr_param)
                    : MAPF_MetamorphicRelationBase(mr_param,
                                                   ObjPool::allocate<OperIncreaseObstacle>(mr_param.MR_para_str)) {
                //{"random":true , "erosion":false, "ratio" : 0.1, "critical" : false}
                //if dilation == 1 then erosion algorithm for obstacle are executed
                //if random == 1 then random part of obstacle are deleted, else all satisfied obstacles are deleted.
                //ratio = [0,1]
                auto j_param = json::parse(operator_ptr->operator_param);
                OperInceaseObstacleParam _param = j_param;

                if (_param.random && !_param.dilation && _param.critical && !_param.shrinked)
                    violation_criteria_func = ViolationCriteria::Equal<double>;
                else
                    violation_criteria_func = ViolationCriteria::Greater<double>;
            }

            bool isRelationSatisfied(const string &origin_result, const vector<string> &follow_results) override {
                RelationAnalyzeInfo info(origin_result, follow_results);

                if (origin_result == "{}" ||
                    std::any_of(
                            follow_results.begin(),
                            follow_results.end(),
                            [](auto &str) { return str == "[]"; }))
                    return false;

                vector<double> lengths;
                lengths.push_back(RelationAnalyzeInfo::GetLength(info.origin));
                for (auto &tmp : info.follows)
                    lengths.push_back(RelationAnalyzeInfo::GetLength(tmp));

                auto j_param = json::parse(operator_ptr->operator_param);
                OperInceaseObstacleParam _param = j_param;

                //TODO 改代码哦！
                //  increase_obstacle_noninteraction, "mr_10-p1", 修改完毕
//                if(_param.random  && !_param.dilation  && _param.critical  && !_param.shrinked)
//                    return std::all_of(lengths.begin() + 1, lengths.end(),
//                                       [&](auto i) {
//                                           return i > lengths[0]  * (1 - info.threshold) &&
//                                                  i < lengths[0]  * (1 + info.threshold);
//                                       });
//                else
//                    return std::all_of(lengths.begin() + 1, lengths.end(),
//                                       [&](auto i) {
//                                           return i >= lengths[0] * (1 - info.threshold);
//                                       });

//                if(_param.random  && !_param.dilation  && _param.critical  && !_param.shrinked)
//                    return ViolationCriteria::Equal(lengths, info.threshold);
//                else
//                    return ViolationCriteria::Greater(lengths, info.threshold);

                return violation_criteria_func(lengths, info.threshold);
            }

            vector<string>
            followCaseParamGenerator(string &origin_case_param, const string &origin_case_result) override {
                if (operator_ptr == NULL)
                    LOG(ERROR) << "@IncreaseObstacle Class, operator_ptr is empty";
                return MAPF_MetamorphicRelationBase::followCaseParamGenerator(origin_case_param, origin_case_result);
            }
        };
    };


}
