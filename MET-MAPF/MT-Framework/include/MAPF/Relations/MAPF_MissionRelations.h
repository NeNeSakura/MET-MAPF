//
// Created by hiccup on 2020/5/14.
//

#pragma once

#include <memory>
#include <string>

#include "boost/format.hpp"
#include "nlohmann/json.hpp"
#include "../Operator/MAPF_Operator.h"
#include "../MAPF_Point2D.hpp"
#include "../../API/IRelation.hpp"
#include "../../API/IOperator.hpp"
#include "../../Param/TestingConfigTree.h"
#include "../../RandomHelper.h"
#include "../../Param/MT_Parameter.hpp"
#include "MAPF_RelationBase.h"
#include "../Execution/MAPF_Execution.h"
#include "../Operator/MAPF_MissionOperator.h"
#include "../DataHandler/MAPF_AnalysisBase.h"

namespace TestingFramework {
    namespace MAPF {

        class Exchange : public MAPF_MetamorphicRelationBase {
        public:
            Exchange(const MR_Parameter &mr_param)
                    : MAPF_MetamorphicRelationBase(mr_param, ObjPool::allocate<OperExchange>(mr_param.MR_para_str)) {
                //empty parameter
                violation_criteria_func = ViolationCriteria::Equal<double>;

            };


            bool isRelationSatisfied(const string &origin_result, const vector<string> &follow_results) override {
                RelationAnalyzeInfo info(origin_result, follow_results);

                vector<double> lengths;
                lengths.push_back(RelationAnalyzeInfo::GetLength(info.origin));
                for (auto &tmp : info.follows)
                    lengths.push_back(RelationAnalyzeInfo::GetLength(tmp));

//                return std::all_of(lengths.begin() + 1, lengths.end(),
//                                   [&](auto i) {
//                                       return i > lengths[0] * (1 - info.threshold) &&
//                                              i < lengths[0] * (1 + info.threshold);
//                                   });
                return violation_criteria_func(lengths, info.threshold);
            }

            vector<string>
            followCaseParamGenerator(string &origin_case_param, const string &origin_case_result) override {
                if (operator_ptr == NULL)
                    LOG(ERROR) << " Operator_ptr is empty";

                return MAPF_MetamorphicRelationBase::followCaseParamGenerator(origin_case_param, origin_case_result);

            }
        };

        class ExchangeRandom : public MAPF_MetamorphicRelationBase {
        public:
            ExchangeRandom(const MR_Parameter &mr_param)
                    : MAPF_MetamorphicRelationBase(mr_param,
                                                   ObjPool::allocate<OperExchangeRandom>(mr_param.MR_para_str)) {
                //{num:5}(int , positive)
                violation_criteria_func = ViolationCriteria::Equal<double>;

            };

            bool isRelationSatisfied(const string &origin_result, const vector<string> &follow_results) override {
                RelationAnalyzeInfo info(origin_result, follow_results);

                vector<double> lengths;
                lengths.push_back(RelationAnalyzeInfo::GetLength(info.origin));
                for (auto &tmp : info.follows)
                    lengths.push_back(RelationAnalyzeInfo::GetLength(tmp));

//                return std::all_of(lengths.begin() + 1, lengths.end(),
//                                   [&](auto i) {
//                                       return i > lengths[0] * (1 - info.threshold) &&
//                                              i < lengths[0] * (1 + info.threshold);
//                                   });

                return violation_criteria_func(lengths, info.threshold);

            }

            vector<string>
            followCaseParamGenerator(string &origin_case_param, const string &origin_case_result) override {
                if (operator_ptr == NULL)
                    LOG(ERROR) << "@ExchangeRandom Class, operator_ptr is empty";
                return MAPF_MetamorphicRelationBase::followCaseParamGenerator(origin_case_param, origin_case_result);

            }
        };

        class DeleteAgent : public MAPF_MetamorphicRelationBase {
        public:
            DeleteAgent(const MR_Parameter &mr_param)
                    : MAPF_MetamorphicRelationBase(mr_param,
                                                   ObjPool::allocate<OperDeleteAgent>(mr_param.MR_para_str)) {
                //dup_num = j_param.at("num").get<int>(); from @MAPF_MetamorphicRelationBase
                //{num:5}(int , positive)
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

            vector<string>
            followCaseParamGenerator(string &origin_case_param, const string &origin_case_result) override {
                if (operator_ptr == NULL)
                    LOG(ERROR) << "@DeleteAgent Class, operator_ptr is empty";
                return MAPF_MetamorphicRelationBase::followCaseParamGenerator(origin_case_param, origin_case_result);

            }
        };

        class AddAgent : public MAPF_MetamorphicRelationBase {
        public:
            AddAgent(const MR_Parameter &mr_param)
                    : MAPF_MetamorphicRelationBase(mr_param,
                                                   ObjPool::allocate<OperAddAgent>(mr_param.MR_para_str)) {
                std::weak_ptr<OperAddAgent> ptr = std::dynamic_pointer_cast<OperAddAgent>(this->operator_ptr);
                //OperAddAgent *ptr = (OperAddAgent *)this->operator_ptr.get();
                auto tmp = ptr.lock();
                tmp->param.is_probe = false;
                tmp->param.is_non_window_probe = true;

                violation_criteria_func = ViolationCriteria::Greater<double>;

            };

            bool isRelationSatisfied(const string &origin_result, const vector<string> &follow_results) override {
                RelationAnalyzeInfo info(origin_result, follow_results);

                vector<double> lengths;
                lengths.push_back(RelationAnalyzeInfo::GetLength(info.origin));
                for (auto &tmp : info.follows)
                    lengths.push_back(RelationAnalyzeInfo::GetLength(tmp));

//                return std::all_of(lengths.begin() + 1, lengths.end(),
//                                   [&](auto i) {
//                                       return i > lengths[0] * (1 - info.threshold);
//                                   });
                return violation_criteria_func(lengths, info.threshold);

            }


            vector<string>
            followCaseParamGenerator(string &origin_case_param, const string &origin_case_result) override {
                if (operator_ptr == NULL)
                    LOG(ERROR) << "@AddAgent Class, operator_ptr is empty";
                return MAPF_MetamorphicRelationBase::followCaseParamGenerator(origin_case_param, origin_case_result);

            }

        };


        class ShrinkStartPos : public MAPF_MetamorphicRelationBase {
        public:
            ShrinkStartPos(const MR_Parameter &mr_param)
                    : MAPF_MetamorphicRelationBase(mr_param,
                                                   ObjPool::allocate<OperMoveAgent>(mr_param.MR_para_str)) {
                //dup_num = j_param.at("num").get<int>(); from @MAPF_MetamorphicRelationBase
                //{num:5}(int , positive)
                violation_criteria_func = ViolationCriteria::Less<double>;
            };

            bool isRelationSatisfied(const string &origin_result, const vector<string> &follow_results) override {
                RelationAnalyzeInfo info(origin_result, follow_results);

                vector<double> lengths;
                lengths.push_back(RelationAnalyzeInfo::GetLength(info.origin));
                for (auto &tmp : info.follows)
                    lengths.push_back(RelationAnalyzeInfo::GetLength(tmp));

                std::shared_ptr<OperMoveAgent> tmp_ptr =
                        std::dynamic_pointer_cast<OperMoveAgent>(this->operator_ptr);

                auto offset = tmp_ptr->move_step;
//                return std::all_of(lengths.begin() + 1, lengths.end(),
//                                   [&](auto i) {
//                                       return i > (lengths[0] - offset) * (1 - info.threshold) &&
//                                              i < (lengths[0] - offset) * (1 + info.threshold);
//                                   });

//                return std::all_of(lengths.begin() + 1, lengths.end(),
//                                   [&](auto i) {
//                                       return i <= (lengths[0]) * (1 + info.threshold);
//                                   });

                return violation_criteria_func(lengths, info.threshold);

            }

            vector<string>
            followCaseParamGenerator(string &origin_case_param, const string &origin_case_result) override {
                if (operator_ptr == NULL)
                    LOG(ERROR) << "@ShrinkStartPos Class, operator_ptr is empty";
                return MAPF_MetamorphicRelationBase::followCaseParamGenerator(origin_case_param, origin_case_result);

            }


        };



        //class DeleteSelectedAgent : public

    }
}
