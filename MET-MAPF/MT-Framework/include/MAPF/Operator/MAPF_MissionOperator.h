//
// Created by hiccup on 2020/5/14.
//

#pragma once

#include <algorithm>


#include "../../API/IOperator.hpp"
#include "../Relations/MAPF_RelationHelper.h"
#include "../../RandomHelper.h"
#include "../MAPF_Point2D.hpp"
#include "../../Param/MR_Parameter.hpp"
#include "../../Param/MT_Parameter.hpp"
#include "MAPF_MissionParam.hpp"
#include "../DataHandler/MAPF_AnalysisBase.h"


namespace TestingFramework {
    namespace MAPF {


        class OperExchange : public IOperator {
        public:
            OperExchangeParam param;

            OperExchange(const string &json_param_str) {
                operator_name = "Exchange_MR1-1";
                operator_param = json_param_str;
                param = json::parse(json_param_str);
            }

            void RelationOperation(string &testcase_param_json, const string &origincase_result = "") override {
                auto scenario = MAPF_RelationHelper::ScenarioJsonReader(testcase_param_json);

                if (param.need_random_seed) {
                    json j = json::parse(testcase_param_json);
                    j["seed"] = RandomHelper::GetRandomInt();       //change execution seed
                    testcase_param_json = j.dump();

                }

                for (auto &item : scenario.task_info)
                    boost::swap(item.start, item.goal);
                MAPF_RelationHelper::ScenarioJsonWriter(testcase_param_json, scenario);
            }
        };

        class OperExchangeRandom : public IOperator {
        public:
            OperExchangeRandomParam param;

            OperExchangeRandom(const string &json_param_str) {
                operator_name = "ExchangeRandom_MR1-2";
                operator_param = json_param_str;
                param = json::parse(json_param_str);
            }

            void RelationOperation(string &testcase_param_json, const string &origincase_result = "") override {
                auto scenario = MAPF_RelationHelper::ScenarioJsonReader(testcase_param_json);

                auto distribution = std::uniform_int_distribution<size_t>(1, scenario.task_info.size());

                std::vector<size_t> idx_vect(scenario.task_info.size());
                std::iota(std::begin(idx_vect), std::end(idx_vect), 0);
                auto &task = scenario.task_info;

                auto num = distribution(RandomHelper::GetCoreGenerator());
                int count = 0;
                //考虑到部分交换后可能多个agent在同一个起点,需要避开这种情况,
                //又需要避免没有任何节点交换，所以写成这个脏样子
                while (count == 0) {
                    count = 0;
                    std::shuffle(idx_vect.begin(), idx_vect.end(), RandomHelper::GetCoreGenerator());
                    for (int i = 0; i < idx_vect.size(); i++) {
                        auto &selected = task[idx_vect[i]];
                        bool flag = true;
                        for (auto &item : task)
                            if (item.start == selected.goal || item.goal == selected.start) {
                                flag = false;
                                break;
                            }
                        if (flag) {
                            count++;
                            boost::swap(selected.start, selected.goal);
                        }
                        if (count >= num)
                            break;
                    }
                }

                MAPF_RelationHelper::ScenarioJsonWriter(testcase_param_json, scenario);

            }
        };


        class OperDeleteAgent : public IOperator {
        public:
            OperDeleteAgentParam param;

            OperDeleteAgent(const string &json_param_str) {
                operator_name = "DeleteAgent_MR-2-1";
                operator_param = json_param_str;
                param = json::parse(json_param_str);
            }

            void RelationOperation(string &testcase_param_json, const string &origincase_result = "") override {
                auto scenario = MAPF_RelationHelper::ScenarioJsonReader(testcase_param_json);

                std::set<size_t> change_set;   //set is ordered.
                //change_set.clear();
                try {

                    if (!param.delete_list.empty()) {
                        for (auto &item : param.delete_list)
                            change_set.insert(item % scenario.task_info.size());

                    } else if (param.ratio >= 0) {
                        double ratio = param.ratio;

                        if (ratio < 1) {
                            int num = std::round(ratio * scenario.task_info.size());
                            while (change_set.size() < num)
                                change_set.insert(RandomHelper::GetRandomInt() % scenario.task_info.size());

                        } else {    // ratio >1
                            double num = std::round(ratio);
                            while (change_set.size() < num)
                                change_set.insert(RandomHelper::GetRandomInt() % scenario.task_info.size());
                        }
                    } else {

                        if (param.ratio < 0)
                            VLOG(2) << "@OperDeleteAgent RelationOperation operator_param, negative ratio value"
                                    << "loading default param";


                        //loading default param
                        auto num = RandomHelper::GetRandomInt() % (scenario.task_info.size() - 1);
                        while (change_set.size() < num)
                            change_set.insert(RandomHelper::GetRandomInt() % scenario.task_info.size());
                    }
                }
                catch (exception &e) {
                    //no delete_list object
                    LOG(WARNING) << e.what();
                    VLOG(2) << "Loading default parameter";
                    change_set.clear();
                    while (change_set.size() < 2)
                        //at least you should have one here
                        change_set.insert(RandomHelper::GetRandomInt() % scenario.task_info.size());
                }

                std::vector<size_t> idx_vect(scenario.task_info.size());
                std::vector<size_t> left_vect;
                std::iota(idx_vect.begin(), idx_vect.end(), 0);
                std::set_difference(idx_vect.begin(), idx_vect.end(), change_set.begin(), change_set.end(),
                                    std::back_inserter(left_vect));

                MAPF_Task task_info;
                for (auto idx : left_vect) {
                    task_info.push_back(scenario.task_info[idx]);
                }
                scenario.task_info = task_info;

                //this is too slow!!!
//                for (auto iter = change_set.rbegin(); iter != change_set.rend(); ++iter) {
//                    scenario.task_info.erase(scenario.task_info.begin() + *iter);
//                }

                MAPF_RelationHelper::ScenarioJsonWriter(testcase_param_json, scenario);

            }

        };


        class OperAddAgent : public IOperator {
        protected:
            std::set<FPoint2> available_pos_set_cache;

            void DataPushBack(unsigned long num,
                              vector<std::pair<FPoint2, FPoint2>> &data,
                              vector<FPoint2> &ss) const;

            void ConstructProbeFromPosCache(unsigned long num,
                                            vector<std::pair<FPoint2, FPoint2>> &data) const;

            void ConstructWindowProbe(unsigned long num, vector<std::pair<FPoint2, FPoint2>> &data,
                                      const MAPF_Task &task_info, const PathsVector &path_info) const;

            std::set<FPoint2> GetWindowPosSet(const FPoint2 &pos) const;

            void ConstructNonProbeAgentTask(unsigned long num, vector<std::pair<FPoint2, FPoint2>> &data) const;

            MAPF_Task
            ConstructAgentTask(MAPF_Scenario &scenario, PathsVector &paths_info, std::vector<size_t> &insert_idx_list);

            std::set<FPoint2> GetAvailablePos(const MAPF_Scenario &scenario) const;


        public:
            OperAddAgentParam param;

            OperAddAgent(const string &json_param_str) {
                operator_name = "AddAgent(for LV4)";
                operator_param = json_param_str;
                param = json::parse(json_param_str);
            }

            void RelationOperation(string &testcase_param_json, const string &origincase_result = "") override {
                auto scenario = MAPF_RelationHelper::ScenarioJsonReader(testcase_param_json);

                //maybe there are more than one agent insert into same position.
                //So, descend order is required here.
                std::vector<size_t> insert_list;

                //delete_list init
                try {

                    if (!param.insert_priority_list.empty()) {
                        for (auto item : param.insert_priority_list)
                            insert_list.push_back(item % scenario.task_info.size());

                    } else if (param.ratio >= 0) {
                        double ratio = param.ratio;

                        if (ratio < 1) {
                            int num = std::round(ratio * scenario.task_info.size());
                            while (insert_list.size() < num)
                                insert_list.push_back(RandomHelper::GetRandomInt() % scenario.task_info.size());

                        } else {    // ratio >1
                            double num = std::round(ratio);
                            while (insert_list.size() < num)
                                insert_list.push_back(RandomHelper::GetRandomInt() % scenario.task_info.size());
                        }
                    } else {

                        if (param.ratio < 0)
                            VLOG(2) << "@OperAddAgent RelationOperation operator_param, negative ratio value"
                                    << "loading default param";

                        //loading default param
                        auto num = RandomHelper::GetRandomInt() % (scenario.task_info.size() - 1);
                        while (insert_list.size() < num)
                            insert_list.push_back(RandomHelper::GetRandomInt() % scenario.task_info.size());
                    }

                    std::sort(insert_list.begin(), insert_list.end(), std::less<size_t>()); //descend order
                }
                catch (exception &e) {
                    //no delete_list object
                    LOG(WARNING) << e.what();
                    VLOG(2) << "Loading default parameter";
                    insert_list.clear();
                    while (insert_list.size() < 2)
                        //at least you should have one here
                        insert_list.push_back(RandomHelper::GetRandomInt() % scenario.task_info.size());
                    std::sort(insert_list.begin(), insert_list.end(), std::less<size_t>()); //descend order
                }

                //delete_list init complete
                available_pos_set_cache.clear();
                auto &task_info = scenario.task_info;
                //get origin path results
                json j = json::parse(origincase_result);
                PathsVector paths_info = j;
                auto task_list = ConstructAgentTask(scenario, paths_info, insert_list);
                for (auto item : task_list) {
                    task_info.insert(task_info.cbegin() + item.idx, item);
                }
                // update new priority
                for (size_t idx = 0; idx < task_info.size(); idx++)
                    task_info[idx].idx = idx;

                available_pos_set_cache.clear();

                MAPF_RelationHelper::ScenarioJsonWriter(testcase_param_json, scenario);

            }

            bool legal_set_cache(unsigned long num, const std::set<FPoint2> &difference_pos_set_cache);
        };

        class OperMoveAgent : public IOperator {

        public:
            OperMoveAgentParam param;
            size_t move_step;

            //Do not support num > 1;
            //Only num == 1 is available.

            OperMoveAgent(const string &json_param_str) {
                operator_name = "MoveAgent_MR-2-?";
                operator_param = json_param_str;
                param = json::parse(json_param_str);
                //idx = 0;
                //free_pos_vect_cache.clear();


                if (param.num != 1) {
                    LOG(WARNING)
                            << "For now ,only param.num == 1 is available, "
                               "because start == goal is easy to hit, which should be avoided.";
                    param.num = 1;
                }
            }
            /*
            void RelationOperation(string &testcase_param_json, const string &origincase_result) {

                //卧草还有这种操作？！
                //#include <boost/assign.hpp>
                //using namespace boost::assign;
                //std::vector<int> input;
                //input += 2,3,4,1,5,6,7,8,9,10;

                if (param.move_pos_type != param.START)
                    LOG(ERROR) << "@OperMoveAgent param error, move_pos_type != param.START";

                //get origin path results
                json j = json::parse(origincase_result);
                PathsVector paths_vect = j;

                // =============================================================
                std::sort(paths_vect.begin(), paths_vect.end(), [](PathInfo &lhs, PathInfo &rhs) -> bool const {
                    return lhs.idx < rhs.idx;
                });

                auto min_it = std::min_element(paths_vect.begin(),
                                               paths_vect.end(),
                                               [](PathInfo &lhs, PathInfo &rhs) {
                                                   return lhs.path.size() < rhs.path.size();
                                               });

                size_t upper_bound = min_it->path.size();
                // =============================================================

                // =============================================================
                int step = std::ceil((upper_bound - 1) * param.ratio);
                auto scenario = MAPF_RelationHelper::ScenarioJsonReader(testcase_param_json);
                while (step > 0) {
                    bool flag = true;
                    for (auto i = 0; i < scenario.task_info.size(); ++i) {
                        if (scenario.task_info[i].idx != paths_vect[i].idx)
                            LOG(ERROR) << format{"idx do not compare! path_vect idx = %1%, taskinfo idx = %2%"} %
                                          paths_vect[i].idx % scenario.task_info[i].idx;
                        else {
                            FPoint2 pos = paths_vect[i].path[step];
                            if (pos == scenario.task_info[i].goal) {
                                flag = false;
                                break;
                            } else {
                                scenario.task_info[i].start = paths_vect[i].path[step];
                            }
                        }
                    }

                    if (!flag) {
                        --step;
                        continue;//invalid move result, try others.
                    } else {
                        move_step = step;
                        break;//get available move result
                    }
                }

                // =============================================================
                MAPF_RelationHelper::ScenarioJsonWriter(testcase_param_json, scenario);

            }
            */

            // new update one
            void RelationOperation(string &testcase_param_json, const string &origincase_result) {

                //卧草还有这种操作？！
                //#include <boost/assign.hpp>
                //using namespace boost::assign;
                //std::vector<int> input;
                //input += 2,3,4,1,5,6,7,8,9,10;

                if (param.move_pos_type != param.START)
                    LOG(ERROR) << "@OperMoveAgent param error, move_pos_type != param.START";

                //get origin path results
                json j = json::parse(origincase_result);
                PathsVector paths_vect = j;

                // =============================================================
                std::sort(paths_vect.begin(), paths_vect.end(), [](PathInfo &lhs, PathInfo &rhs) -> bool const {
                    return lhs.idx < rhs.idx;
                });

                auto min_it = std::min_element(paths_vect.begin(),
                                               paths_vect.end(),
                                               [](PathInfo &lhs, PathInfo &rhs) {
                                                   return lhs.path.size() < rhs.path.size();
                                               });
                auto max_it = std::max_element(paths_vect.begin(),
                                               paths_vect.end(),
                                               [](PathInfo &lhs, PathInfo &rhs) {
                                                   return lhs.path.size() < rhs.path.size();
                                               });

                size_t max_upper_bound = max_it->path.size();
                size_t min_upper_bound = min_it->path.size();
                // =============================================================

                // =============================================================
                int step = std::ceil((max_upper_bound - 1) * param.ratio);
                if (step > min_upper_bound - 1)
                    step = min_upper_bound - 1;
                auto scenario = MAPF_RelationHelper::ScenarioJsonReader(testcase_param_json);
                while (step >= 0) {
                    bool flag = true;
                    for (auto i = 0; i < scenario.task_info.size(); ++i) {
                        if (scenario.task_info[i].idx != paths_vect[i].idx)
                            LOG(ERROR) << format{"idx do not compare! path_vect idx = %1%, taskinfo idx = %2%"} %
                                          paths_vect[i].idx % scenario.task_info[i].idx;
                        else {
                            FPoint2 pos = paths_vect[i].path[step];
                            if (pos == scenario.task_info[i].goal) {
                                flag = false;
                                break;
                            } else {
                                scenario.task_info[i].start = paths_vect[i].path[step];
                            }
                        }
                    }

                    if(IsScenarioTaskConflict(scenario))
                    {
                        --step;
                        continue;
                    }

                    if (!flag) {
                        --step;
                        continue;//invalid move result, try others.
                    } else {
                        move_step = step;
                        break;//get available move result
                    }
                }
                if(step == 0)
                    LOG(WARNING) << "@OperMoveAgent param Warning, step is 0 , no metamorphic relation is operated";

                // =============================================================
                MAPF_RelationHelper::ScenarioJsonWriter(testcase_param_json, scenario);

            }

            bool IsScenarioTaskConflict(const MAPF_Scenario &scenario) const {
                for (int i = 0; i < scenario.task_info.size(); ++i) {
                    for (int j = i + 1; j < scenario.task_info.size(); ++j) {
                        if (scenario.task_info[i].start == scenario.task_info[j].start ||
                            scenario.task_info[i].goal == scenario.task_info[j].goal) {

                            return true;
                        }
                    }
                }
                return false;
            }

        };
    }
}

