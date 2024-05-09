//
// Created by hiccup on 2020/5/15.
//

#pragma once


#include "../../Param/MR_Parameter.hpp"
#include "../../API/IRelation.hpp"
#include "MAPF_RelationBase.h"
#include "../Operator/MAPF_InterativeOperator.h"
#include "../DataHandler/MAPF_AnalysisBase.h"
#include "../Operator/MAPF_MissionOperator.h"
#include "../../../../mem_pool/include/ObjPool.hpp"

namespace TestingFramework {
    namespace MAPF {

        class PriorityExchange : public MAPF_MetamorphicRelationBase {
        public:
            PriorityExchange(const MR_Parameter &mr_param)
                    : MAPF_MetamorphicRelationBase(mr_param,
                                                   ObjPool::allocate<OperPriorityExchange>(mr_param.MR_para_str)) {
                //num:? (int , positive) or null
                //delete_list: [ int,int,...,int] or null
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
                return violation_criteria_func(lengths,info.threshold);

            }

            vector<string>
            followCaseParamGenerator(string &origin_case_param, const string &origin_case_result) override {
                if (operator_ptr == NULL)
                    LOG(ERROR) << "@PriorityExchange Class, operator_ptr is empty";
                return MAPF_MetamorphicRelationBase::followCaseParamGenerator(origin_case_param, origin_case_result);

            }
        };

        class AddRandomPriorityProbe : public MAPF_MetamorphicRelationBase {
        public:
            AddRandomPriorityProbe(const MR_Parameter &mr_param)
                    : MAPF_MetamorphicRelationBase(mr_param,
                                                   ObjPool::allocate<OperAddAgent>(mr_param.MR_para_str)) {
                std::weak_ptr<OperAddAgent> ptr = std::dynamic_pointer_cast<OperAddAgent>(this->operator_ptr);
                //OperAddAgent *ptr = (OperAddAgent *)this->operator_ptr.get();
                auto tmp = ptr.lock();
                tmp->param.is_probe = true;
                tmp->param.is_non_window_probe =true;

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
                return violation_criteria_func(lengths,info.threshold);

            }

            vector<string>
            followCaseParamGenerator(string &origin_case_param, const string &origin_case_result) override {
                if (operator_ptr == NULL)
                    LOG(ERROR) << "@AddRandomPriorityProbe Class, operator_ptr is empty";
                return MAPF_MetamorphicRelationBase::followCaseParamGenerator(origin_case_param, origin_case_result);

            }

        };



        class AddWindowedPriorityProbe : public MAPF_MetamorphicRelationBase {
        public:
            AddWindowedPriorityProbe(const MR_Parameter &mr_param)
                    : MAPF_MetamorphicRelationBase(mr_param,
                                                   ObjPool::allocate<OperAddAgent>(mr_param.MR_para_str)) {
                std::weak_ptr<OperAddAgent> ptr = std::dynamic_pointer_cast<OperAddAgent>(this->operator_ptr);
                //OperAddAgent *ptr = (OperAddAgent *)this->operator_ptr.get();
                auto tmp = ptr.lock();
                tmp->param.is_probe = true;
                tmp->param.is_non_window_probe =false;

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

                return violation_criteria_func(lengths,info.threshold);

            }

            vector<string>
            followCaseParamGenerator(string &origin_case_param, const string &origin_case_result) override {
                if (operator_ptr == NULL)
                    LOG(ERROR) << "@AddWindowedPriorityProbe Class, operator_ptr is empty";
                return MAPF_MetamorphicRelationBase::followCaseParamGenerator(origin_case_param, origin_case_result);

            }

        };



//        class PriorityExchangeRandom : public MAPF_MetamorphicRelationBase {
//        public:
//            PriorityExchangeRandom(const MR_Parameter &mr_param)
//                    : MAPF_MetamorphicRelationBase(mr_param,
//                                                   nullptr) {
//                //num:? (int , positive) or null
//                //delete_list: [ int,int,...,int] or null
//                json j = json::parse(mr_param.MR_para_str);
//                try {
//                    j.at("delete_list"); //check if delete_list exists
//                }
//                catch(...) {
//                    j["delete_list"]= json::array();
//                }
//                this->operator_ptr = ObjPool::allocate<OperPriorityExchange>(j.dump());
//            };
//
//            bool isRelationSatisfied(const string &origin_result, const vector <string> &follow_results) override {
//                RelationAnalyzeInfo info(origin_result, follow_results);
//
//                vector<int> lengths;
//                lengths.push_back(RelationAnalyzeInfo::GetLength(info.origin));
//                for (auto &tmp : info.follows)
//                    lengths.push_back(RelationAnalyzeInfo::GetLength(tmp));
//
//                return std::all_of(lengths.begin() + 1, lengths.end(),
//                                   [&](auto i) {
//                                       return i > lengths[0] * (1 - info.threshold) &&
//                                              i < lengths[0] * (1 + info.threshold);
//                                   });
//
//            }
//
//            vector<string>
//            followCaseParamGenerator(string &origin_case_param, const string &origin_case_result) override {
//                if (operator_ptr == NULL)
//                    LOG(ERROR) << "@PriorityExchangeRandom Class, operator_ptr is empty";
//                return MAPF_MetamorphicRelationBase::followCaseParamGenerator(origin_case_param, "");
//
//            }
//        };
    }
}