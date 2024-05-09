//
// Created by hiccup on 2020/5/16.
//

#pragma once

#include "../../API/IOperator.hpp"
#include "../Relations/MAPF_RelationHelper.h"
#include "MAPF_InterativeParam.hpp"
#include <algorithm>
#include <set>

namespace TestingFramework {
    namespace MAPF {
        class OperPriorityExchange : public IOperator {
        public:
            OperPriorityExchangeParam param;

            OperPriorityExchange(const string &json_param_str) {
                operator_name = "PriorityExchange_MR4-1";
                operator_param = json_param_str;
                param = json::parse(json_param_str);
            }

            void RelationOperation(string &testcase_param_json, const string &origincase_result = "") override {
                auto scenario = MAPF_RelationHelper::ScenarioJsonReader(testcase_param_json);

//                std::set<int> change_set;   //set is ordered.
//
//                json j_param = json::parse(operator_param);
//                if(j_param.find("change_list") != j_param.end())
//                {
//                    json &j_change_list = j_param.at("change_list");
//                    for (auto &item : j_change_list)
//                        change_set.insert(item.get<int>() % scenario.task_info.size());
//                } else{
//                    int num =0;
//                    double ratio = -1;
//                    if(j_param.find("ratio") != j_param.end())
//                        ratio = j_param["ratio"].get<double>();
//
//                    if(ratio >1 || ratio <0)
//                        num = RandomHelper::GetRandomInt() % scenario.task_info.size();
//                    else
//                        num = std::round(ratio * scenario.task_info.size());
//
//                    while(change_set.size()<num)
//                        change_set.insert(RandomHelper::GetRandomInt() % scenario.task_info.size());
//                }

                std::set<int> change_set;   //set is ordered.

                if(!param.change_list.empty() )
                {
                    for (auto &item : param.change_list)
                        change_set.insert(item % scenario.task_info.size());
                } else{
                    int num =0;
                    double ratio = param.ratio;
                    if(ratio >1 || ratio <0)
                        num = RandomHelper::GetRandomInt() % scenario.task_info.size();
                    else
                        num = std::round(ratio * scenario.task_info.size());

                    while(change_set.size()<num)
                        change_set.insert(RandomHelper::GetRandomInt() % scenario.task_info.size());
                }


                vector<int> ordered_vect;
                vector<int> shuffle_vect;
                std::copy(change_set.begin(), change_set.end(), std::back_inserter(ordered_vect));
                std::copy(change_set.begin(), change_set.end(), std::back_inserter(shuffle_vect));

                if(change_set.size() == 2)
                    std::reverse(shuffle_vect.begin(),shuffle_vect.end());
                else
                    std::shuffle(shuffle_vect.begin(),shuffle_vect.end(),RandomHelper::GetCoreGenerator());

                MAPF_Task task = scenario.task_info; // deep copy;

                for (int i =0 ; i< shuffle_vect.size();i++)
                {
                    // keep origin idx ,then we could know which agent moved;
                    //boost::swap(scenario.task_info[ordered_vect[idx]].idx, task[shuffle_vect[idx]].idx);
                    boost::swap(scenario.task_info[ordered_vect[i]], task[shuffle_vect[i]]);
                }

                MAPF_RelationHelper::ScenarioJsonWriter(testcase_param_json, scenario);
            }
        };


    }
}