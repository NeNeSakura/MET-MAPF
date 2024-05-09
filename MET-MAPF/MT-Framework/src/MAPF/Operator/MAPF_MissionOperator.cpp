//
// Created by hiccup on 2020/5/14.
//

#include "../../../include/MAPF/Operator/MAPF_MissionOperator.h"

namespace TestingFramework {
    namespace MAPF {
        void OperAddAgent::DataPushBack(unsigned long num, vector<std::pair<FPoint2, FPoint2>> &data,
                                        vector<FPoint2> &ss) const {
            data.clear();
            size_t count = 0;
            auto iter = ss.begin();
            std::set<FPoint2> occupied;

            while (count < num && iter != ss.end()) {
#ifdef _SUT_VERSION_
                //construct same end pos( same pos is legal for MAPF testing code in V2.0)
                std::vector<FPoint2> candidate = {*iter + FPoint2(0, 0)};
#else
                //construct adjacent end pos( same pos is not legal for MAPF testing code)
                std::vector<FPoint2> candidate = {*iter + FPoint2(0, 1), *iter + FPoint2(0, -1),
                                                  *iter + FPoint2(1, 0), *iter + FPoint2(-1, 0)};
#endif

                auto it = std::find_first_of(available_pos_set_cache.begin(),
                                             available_pos_set_cache.end(),
                                             candidate.begin(),
                                             candidate.end());
                if (it != available_pos_set_cache.end() &&
                    occupied.find(*it) == occupied.end() &&
                    occupied.find(*iter) == occupied.end()) {
                    data.push_back({*iter, *it});
                    occupied.insert(*it);
                    occupied.insert(*iter);
                    count++;
                    iter++;

                } else {
                    iter++;
                    continue;
                }
            }
            if (iter == ss.end() && count < num) {
                LOG(WARNING) << "@OperAddAgent class -> @ConstructAgent \n"
                                "Task count number is less than expected nums";
            };
        }

        void
        OperAddAgent::ConstructProbeFromPosCache(unsigned long num, vector<std::pair<FPoint2, FPoint2>> &data) const {
            std::vector<FPoint2> ss;
            std::copy(available_pos_set_cache.begin(),
                      available_pos_set_cache.end(),
                      std::inserter(ss, ss.begin()));
            std::shuffle(ss.begin(), ss.end(), RandomHelper::GetCoreGenerator());

            DataPushBack(num, data, ss);
        }

        void OperAddAgent::ConstructWindowProbe(unsigned long num, vector<std::pair<FPoint2, FPoint2>> &data,
                                                const MAPF_Task &task_info, const PathsVector &path_info) const {
            if (param.window_size == 0) {
                LOG(WARNING) << "@OperAddAgent Illegal window_size in param, window_size == 0. Set window_size = 1";
                param.window_size == 1;
            }
            std::set<FPoint2> pos_set;
            std::vector<FPoint2> ss;
            std::set<FPoint2> start_target_set;


//            for (auto &item : task_info) {
//                auto start_set = GetWindowPosSet(item.start);
//                pos_set.insert(start_set.begin(), start_set.end());
//                auto goal_set = GetWindowPosSet(item.goal);
//                pos_set.insert(goal_set.begin(), goal_set.end());
//            }


            // Get path info first , then construct the PosSet of paths.
            // Last throw the start and target pos as the final pos_set
            for (auto &path : path_info)
                for (auto &pos : path.path) {
                    auto tmp_set = GetWindowPosSet(pos);
                    pos_set.insert(tmp_set.begin(), tmp_set.end());
                }

            for (auto &item : task_info) {
                start_target_set.insert(item.start);
                start_target_set.insert(item.goal);
            }

            std::set_difference(pos_set.begin(), pos_set.end(), start_target_set.begin(), start_target_set.end(),
                                std::inserter(ss, ss.begin()));

            //std::copy(pos_set.begin(), pos_set.end(), std::inserter(ss, ss.begin()));

            std::shuffle(ss.begin(), ss.end(), RandomHelper::GetCoreGenerator());

            DataPushBack(num, data, ss);

        }

        std::set<FPoint2> OperAddAgent::GetWindowPosSet(const FPoint2 &pos) const {
            std::set<FPoint2> available_pos;
            int size = param.window_size;
            for (int x = -size; x <= size; x++)
                for (int y = -size - x; y <= size + x; y++)
                    if (available_pos_set_cache.find(pos + FPoint2(x, y)) != available_pos_set_cache.end())
                        available_pos.insert(pos + FPoint2(x, y));

            return available_pos;
        }

        void
        OperAddAgent::ConstructNonProbeAgentTask(unsigned long num, vector<std::pair<FPoint2, FPoint2>> &data) const {
            std::vector<FPoint2> ss(available_pos_set_cache.size());
            std::vector<FPoint2> gs(available_pos_set_cache.size());
            bool flg;

            std::copy(available_pos_set_cache.begin(), available_pos_set_cache.end(), ss.begin());
            std::copy(available_pos_set_cache.begin(), available_pos_set_cache.end(), gs.begin());

            while (true) {
                data.clear();
                std::shuffle(ss.begin(), ss.end(), RandomHelper::GetCoreGenerator());
                std::shuffle(gs.begin(), gs.end(), RandomHelper::GetCoreGenerator());
                flg = true;
                for (auto i = 0; i < num; ++i) {
                    if (ss[i] != gs[i]) {
                        data.push_back({ss[i], gs[i]});
                    } else {
                        flg = false;
                        break;
                    }
                }
                if (flg) break;
            }
        }

        MAPF_Task OperAddAgent::ConstructAgentTask(MAPF_Scenario &scenario, PathsVector &path_info,
                                                   std::vector<size_t> &insert_idx_list) {

            auto result = MAPF_Task();
            //map reader
            if (available_pos_set_cache.empty())
                available_pos_set_cache = GetAvailablePos(scenario);

            if (param.insert_item_list.empty()) {

                //auto distribution = std::uniform_int_distribution<size_t>(0, available_pos_set_cache.size());
                auto num = insert_idx_list.size();

                vector<std::pair<FPoint2, FPoint2>> data;

                if (param.is_probe) {
                    if (param.is_non_window_probe) {
                        std::set<FPoint2> path_pos_set_cache, difference_pos_set_cache;
                        for (auto &path: path_info)
                            for (auto &pos : path.path)
                                path_pos_set_cache.insert(pos);
                        std::set_difference(available_pos_set_cache.begin(), available_pos_set_cache.end(),
                                            path_pos_set_cache.begin(), path_pos_set_cache.end(),
                                            std::inserter(difference_pos_set_cache, difference_pos_set_cache.begin()));
                        //check probe available?
                        if (legal_set_cache(num, difference_pos_set_cache))
                            available_pos_set_cache = difference_pos_set_cache;
                        else VLOG(1) << "Construct Non Interaction probe failed , no enough probe space to realize";

                        ConstructProbeFromPosCache(num, data);
                    } else //(!param.is_non_window_probe)
                        ConstructWindowProbe(num, data, scenario.task_info, path_info);
                } else {

                    ConstructNonProbeAgentTask(num, data);
                }

                if (data.size() != insert_idx_list.size())
                    LOG(ERROR) << "@OperAddAgent , illegal list size. data.size() != insert_idx_list.size()";

                std::vector<MAPF_TaskItem> result_items;

                for (auto i = 0; i < std::min(insert_idx_list.size(), data.size()); i++)
                    result_items.push_back({data[i].first, data[i].second, insert_idx_list[i]});

                return result_items;

            } else {
                if (available_pos_set_cache.empty())
                    available_pos_set_cache = GetAvailablePos(scenario);

                MAPF_Task delete_task;
                for (auto item :param.insert_item_list) {
                    if (available_pos_set_cache.find(item.start) == available_pos_set_cache.end() ||
                        available_pos_set_cache.find(item.goal) == available_pos_set_cache.end()) {
                        {
                            LOG(WARNING) << boost::format{"@OperAddAgent, illegal input insert_item_list item. "
                                                          "Delete Agent: idx:%1%, start:%2%, goal:%3%."} %
                                            item.idx % item.start % item.goal;

                            delete_task.push_back(item);
                        }
                    }
                }

                auto iter = param.insert_item_list.begin();
                for (auto item : delete_task) {
                    iter = std::remove(param.insert_item_list.begin(), param.insert_item_list.end(), item);
                }
                param.insert_item_list.erase(iter, param.insert_item_list.end());
                return param.insert_item_list;
            }

        }

        bool
        OperAddAgent::legal_set_cache(unsigned long num, const std::set<FPoint2> &difference_pos_set_cache) {
            size_t count = 0;
            auto tmp = param.window_size;
            param.window_size = 1;
//            param.window_size = 1;
            for (const FPoint2 &pos : difference_pos_set_cache) {
                auto adj_pos = GetWindowPosSet(pos);
                if (adj_pos.size() != 0)
                    count++;
                if (count >= num) {
                    param.window_size = tmp;
                    return true;
                }

            }
            param.window_size = tmp;
            return false;
        }

        std::set<FPoint2> OperAddAgent::GetAvailablePos(const MAPF_Scenario &scenario) const {
            auto &map_data = scenario.map_info.map_data;
            std::set<FPoint2> occupied_pos_set;
            std::set<FPoint2> safe_pos_set;
            std::set<FPoint2> available_pos_set;

            for (auto y = 0; y < scenario.map_info.height; y++) {
                for (auto x = 0; x < scenario.map_info.width; x++) {
                    if (map_data[y][x] == '.')
                        safe_pos_set.insert(FPoint2(x, y));
                }
            }

            for (auto &item : scenario.task_info) {
                occupied_pos_set.insert(item.start);
                occupied_pos_set.insert(item.goal);
            }

            std::set_difference(safe_pos_set.begin(), safe_pos_set.end(), occupied_pos_set.begin(),
                                occupied_pos_set.end(),
                                std::inserter(available_pos_set, available_pos_set.begin()));
            return available_pos_set;
        }
    }

}