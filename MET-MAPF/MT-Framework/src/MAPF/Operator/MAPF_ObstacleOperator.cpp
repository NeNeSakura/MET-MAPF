//
// Created by hiccup on 2020/5/20.
//

#include "../../../include/MAPF/Operator/MAPF_ObstacleOperator.h"
#include "../../../include/MAPF/DataHandler/MAPF_AnalysisBase.h"

namespace TestingFramework {
    namespace MAPF {


        void OperDecreaseObstacle::RelationOperation(string &testcase_param_json, const string &origincase_result) {


            bool is_random = param.random;
            bool is_erosion = param.erosion;
            double ratio = param.ratio;

//            bool is_non_window_probe = false;
//            bool is_erosion = false;
//            double ratio = 0.1;
//            json j_param = json::parse(operator_param);
//            if (j_param.find("random") != j_param.end()) {
//                j_param["random"].get_to(is_non_window_probe);
//            }
//            if (j_param.find("erosion") != j_param.end())
//                j_param["erosion"].get_to(is_erosion);
//            if (j_param.find("ratio") != j_param.end())
//                j_param["ratio"].get_to(ratio);


            // =============================================================

            auto scenario = MAPF_RelationHelper::ScenarioJsonReader(testcase_param_json);
            auto &map_data = scenario.map_info.map_data;
            std::vector<FPoint2> obstacle_pos_vect;
            for (size_t j = 0; j < map_data.size(); ++j)
                for (size_t i = 0; i < map_data[j].size(); ++i)
                    if (map_data[j][i] == 'T' || map_data[j][i] == '@')
                        obstacle_pos_vect.push_back(FPoint2(i, j));

            // =============================================================


            // TODO erosion part
            if (!is_random && is_erosion) {
                auto erosion_pos_set = GetErosionPos(origincase_result, obstacle_pos_vect);
                for (auto iter = erosion_pos_set.begin(); iter != erosion_pos_set.end(); ++iter)
                    map_data[iter->y][iter->x] = '.';
//                LOG(WARNING) << "EMPTY CODE BRANCH!!";

            } else if (is_random && is_erosion) {

                auto erosion_pos_set = GetErosionPos(origincase_result, obstacle_pos_vect);
                int num = 0;
                if (ratio >= 1 || ratio < 0)
                    num = RandomHelper::GetRandomInt() % erosion_pos_set.size();
                else
                    num = std::round(erosion_pos_set.size() * ratio);

                std::vector<FPoint2> value_set;
                std::copy(erosion_pos_set.begin(), erosion_pos_set.end(), std::back_inserter(value_set));

                std::vector<int> idx_v(erosion_pos_set.size());
                std::iota(idx_v.begin(), idx_v.end(), 0);
                std::shuffle(idx_v.begin(), idx_v.end(), RandomHelper::GetCoreGenerator());

                for (int i = 0; i < num; ++i) {
                    auto &selected_pos = value_set[idx_v[i]];
                    map_data[selected_pos.y][selected_pos.x] = '.';
                }

//                LOG(WARNING) << "EMPTY CODE BRANCH!!";

            } else if (!is_random && !is_erosion) {
                for (auto iter = obstacle_pos_vect.begin(); iter != obstacle_pos_vect.end(); ++iter)
                    map_data[iter->y][iter->x] = '.';
            } else {
                //default operation
                int num = 0;
                if (ratio >= 1 || ratio < 0)
                    num = RandomHelper::GetRandomInt() % obstacle_pos_vect.size();
                else
                    num = std::round(obstacle_pos_vect.size() * ratio);
                std::vector<int> v(obstacle_pos_vect.size());
                std::iota(v.begin(), v.end(), 0);
                std::shuffle(v.begin(), v.end(), RandomHelper::GetCoreGenerator());

                for (int i = 0; i < num; ++i) {
                    auto &selected_pos = obstacle_pos_vect[v[i]];
                    map_data[selected_pos.y][selected_pos.x] = '.';
                }
            }

            MAPF_RelationHelper::ScenarioJsonWriter(testcase_param_json, scenario);
        }

        std::set<FPoint2> OperDecreaseObstacle::GetErosionPos(const string &origincase_result,
                                                              vector<FPoint2> &obstacle_pos_vect) const {//get origin path results
            json j = json::parse(origincase_result);
            PathsVector paths_vect = j;
            std::set<FPoint2> path_pos_set_cache, obstacle_cache;
            for (auto &path: paths_vect)
                for (auto &pos : path.path)
                    path_pos_set_cache.insert(pos);
            obstacle_cache.insert(obstacle_pos_vect.begin(),obstacle_pos_vect.end());
//            std::cout << path_pos_set_cache.size() << " " << obstacle_cache.size();

            std::set<FPoint2> erosion_pos;
            for (const auto &pos : path_pos_set_cache) {
//                int size = 1;
//                for (int x = -size; x <= size; x++)
//                    for (int y = -size - x; y <= size + x; y++)
//                        if (std::find(obstacle_pos_vect.begin(), obstacle_pos_vect.end(), (pos + FPoint2(x, y))) !=
//                            obstacle_pos_vect.end())
//                            erosion_pos.insert(pos + FPoint2(x, y));
                vector<FPoint2> tmp_vect{pos + FPoint2(0, 1),
                                         pos + FPoint2(0, -1),
                                         pos + FPoint2(-1, 0),
                                         pos + FPoint2(1, 0)};
                for (const auto &tmp : tmp_vect) {
                    if (std::find(obstacle_cache.begin(), obstacle_cache.end(), tmp) !=
                            obstacle_cache.end())
                        erosion_pos.insert(tmp);
                }

            }
            return erosion_pos;
        }


        void OperIncreaseObstacle::RelationOperation(string &testcase_param_json, const string &origincase_result) {

            bool is_random = param.random;
            bool is_dilation = param.dilation;
            bool is_critical = param.critical;
            bool is_shrinked = param.shrinked;
            double ratio = param.ratio;

            //==============================================================
//            bool is_non_window_probe = true;
//            bool is_dilation = false;
//            bool is_critical = false;
//            bool is_shrinked = true;
//            double ratio = 0.1;
//
//            json j_param = json::parse(operator_param);
//
//            if (j_param.find("random") != j_param.end())
//                j_param["random"].get_to(is_non_window_probe);
//            if (j_param.find("dilation") != j_param.end())
//                j_param["dilation"].get_to(is_dilation);
//            if (j_param.find("ratio") != j_param.end())
//                j_param["ratio"].get_to(ratio);
//            if (j_param.find("critical") != j_param.end())
//                j_param["critical"].get_to(is_critical);
//            if (j_param.find("is_shrinked") != j_param.end())
//                j_param["is_shrinked"].get_to(is_shrinked);


            // =============================================================

            auto scenario = MAPF_RelationHelper::ScenarioJsonReader(testcase_param_json);
            auto &map_data = scenario.map_info.map_data;
            std::vector<FPoint2> free_pos_vect;
            std::set<FPoint2> obstacle_pos_vect;
            for (size_t j = 0; j < scenario.map_info.height; ++j)
                for (size_t i = 0; i < scenario.map_info.width; ++i) {
                    if (map_data[j][i] == '.')
                        free_pos_vect.push_back(FPoint2(i, j));
                    else
                        obstacle_pos_vect.insert(FPoint2(i, j));
                }

            std::set<FPoint2> forbidden_pos_set;

            if(is_random)
            {
//                for (auto &p : free_pos_vect) {
//                    for (int i = -1; i <= 1; i++) {
//                        if (obstacle_pos_vect.find(FPoint2{p.x + i, p.y + 1}) != obstacle_pos_vect.end() &&
//                            obstacle_pos_vect.find(FPoint2{p.x + i, p.y - 1}) != obstacle_pos_vect.end())
//                            forbidden_pos_set.insert(p);
//                        if (obstacle_pos_vect.find(FPoint2{p.x + 1, p.y + i}) != obstacle_pos_vect.end() &&
//                            obstacle_pos_vect.find(FPoint2{p.x - 1, p.y + i}) != obstacle_pos_vect.end())
//                            forbidden_pos_set.insert(p);
//                    }
//
//                    vector<int> tmp_v = {-1,1};
//                    for (int i : tmp_v) {
//                        for(int j : tmp_v){
//                            if (obstacle_pos_vect.find(FPoint2{p.x + i, p.y + j}) != obstacle_pos_vect.end() &&
//                                obstacle_pos_vect.find(FPoint2{p.x , p.y + j}) != obstacle_pos_vect.end())
//                                forbidden_pos_set.insert(p);
//                            if (obstacle_pos_vect.find(FPoint2{p.x + j, p.y + i}) != obstacle_pos_vect.end() &&
//                                obstacle_pos_vect.find(FPoint2{p.x + j, p.y}) != obstacle_pos_vect.end())
//                                forbidden_pos_set.insert(p);
//                        }
//                    }
//                }

                for (auto &p : free_pos_vect) {

                    vector<int> tmp_v = {-1,0,1};
                    for (int i : tmp_v) {
                        for(int j : tmp_v){
//                            if(i == 0 && j==0 )
//                                continue;
                            if (obstacle_pos_vect.find(FPoint2{p.x + i, p.y + j}) != obstacle_pos_vect.end())
                                forbidden_pos_set.insert(p);
                        }
                    }
                }


            }

            auto &task = scenario.task_info;
            for (auto &info : task) {
                if (is_shrinked) {
                    const auto p_start = info.start;
                    vector<FPoint2> start_forbidden_pos = {p_start,
                                                           FPoint2{p_start.x - 1, p_start.y},
                                                           FPoint2{p_start.x + 1, p_start.y},
                                                           FPoint2{p_start.x, p_start.y - 1},
                                                           FPoint2{p_start.x, p_start.y + 1}};

                    const auto p_goal = info.goal;
                    vector<FPoint2> goal_forbidden_pos = {p_goal,
                                                          FPoint2{p_goal.x - 1, p_goal.y},
                                                          FPoint2{p_goal.x + 1, p_goal.y},
                                                          FPoint2{p_goal.x, p_goal.y - 1},
                                                          FPoint2{p_goal.x, p_goal.y + 1}};

                    forbidden_pos_set.insert(start_forbidden_pos.begin(), start_forbidden_pos.end());
                    forbidden_pos_set.insert(goal_forbidden_pos.begin(), goal_forbidden_pos.end());
                } else {
                    const auto p_start = info.start;
                    const auto p_goal = info.goal;

                    forbidden_pos_set.insert(p_start);
                    forbidden_pos_set.insert(p_goal);
                }

            }


            if (is_critical) {
                try {
                    json j = json::parse(origincase_result);
                    PathsVector paths_vect = j;
                    for (auto &path : paths_vect)
                        forbidden_pos_set.insert(path.path.begin(), path.path.end());
                }
                catch (exception &e) {
                    LOG(WARNING) << format{"Something wrong with the string origincase_result : %1% "} % e.what();

                }
            }

            // =============================================================

            vector<FPoint2> shrinked_pos_vect;

            std::sort(free_pos_vect.begin(), free_pos_vect.end());
            std::set_difference(free_pos_vect.cbegin(), free_pos_vect.cend(),
                                forbidden_pos_set.cbegin(), forbidden_pos_set.cend(),
                                std::back_inserter(shrinked_pos_vect));


            // TODO delation part
            if (!is_random && is_dilation) {
                if (is_critical == true)
                    LOG(ERROR) << "ILLEGAL PARAMETER SETTING!";
                else {
                    vector<FPoint2> paths_legal_pos;
                    std::set<FPoint2> paths_set;
                    json j = json::parse(origincase_result);
                    PathsVector paths_vect = j;
                    for (auto &path : paths_vect)
                        for (auto &pos : path.path) {
                            paths_set.insert(
                                    {pos,
                                     pos + FPoint2{0, 1},
                                     pos + FPoint2{0, -1},
                                     pos + FPoint2{1, 0},
                                     pos + FPoint2{-1, 0}});
                        }

                    std::set_intersection(shrinked_pos_vect.begin(), shrinked_pos_vect.end(),
                                          paths_set.cbegin(), paths_set.cend(),
                                          std::back_inserter(paths_legal_pos));

//                    int num = 0;
//                    if (ratio >= 1 || ratio < 0)
//                        num = RandomHelper::GetRandomInt() % shrinked_pos_vect.size();
//                    else
//                        num = std::round(shrinked_pos_vect.size() * ratio);
                    std::vector<int> v(paths_legal_pos.size());
                    std::iota(v.begin(), v.end(), 0);
                    std::shuffle(v.begin(), v.end(), RandomHelper::GetCoreGenerator());

                    for (auto idx : v) {
                        auto &selected_pos = paths_legal_pos[idx];
                        vector<FPoint2> tmp_square = {FPoint2{selected_pos.x - 1, selected_pos.y - 1},
                                                      FPoint2{selected_pos.x, selected_pos.y - 1},
                                                      FPoint2{selected_pos.x + 1, selected_pos.y - 1},

                                                      FPoint2{selected_pos.x - 1, selected_pos.y},
                                                      selected_pos,
                                                      FPoint2{selected_pos.x + 1, selected_pos.y},

                                                      FPoint2{selected_pos.x - 1, selected_pos.y + 1},
                                                      FPoint2{selected_pos.x, selected_pos.y + 1},
                                                      FPoint2{selected_pos.x + 1, selected_pos.y + 1}
                        };
                        if (std::all_of(tmp_square.begin(), tmp_square.end(),
                                        [&](FPoint2 &lhs) -> bool {
                                            return std::find(paths_legal_pos.begin(), paths_legal_pos.end(), lhs) !=
                                                   paths_legal_pos.end();
                                        })) {
                            map_data[selected_pos.y][selected_pos.x] = 'T';
                            break;
                        }

                    }


                }
            } else if (is_random && is_dilation) {
                LOG(WARNING) << "EMPTY CODE BRANCH!!";
                // random select
                // get the first obstacle



            } else if (!is_random && !is_dilation) {
                LOG(WARNING) << "EMPTY CODE BRANCH!!";
            } else {
                // random select
                // non dilation

                //default operation
                int num = 0;
                if (ratio >= 1 || ratio < 0)
                    num = RandomHelper::GetRandomInt() % shrinked_pos_vect.size();
                else
                    num = std::round(shrinked_pos_vect.size() * ratio);
                std::vector<int> v(shrinked_pos_vect.size());
                std::iota(v.begin(), v.end(), 0);
                std::shuffle(v.begin(), v.end(), RandomHelper::GetCoreGenerator());


                vector<int> tmp_v = {-1,0,1};
                for (int i = 0; i < num; ++i) {
                    auto selected_pos = shrinked_pos_vect[v[i]];
                    //To avoid Graph connectivity judgment.

                    bool pos_available = true;
                    //check adjacent position.
                    for (int x : tmp_v) {
                        for(int y : tmp_v){
                            selected_pos.x += x;
                            selected_pos.y += y;
                            if (obstacle_pos_vect.find(selected_pos) != obstacle_pos_vect.end())
                                pos_available =false;
                        }
                    }

                    if(pos_available)
                    {
                        map_data[selected_pos.y][selected_pos.x] = 'T';
                        obstacle_pos_vect.insert(selected_pos);
                    }

                }
            }

            MAPF_RelationHelper::ScenarioJsonWriter(testcase_param_json, scenario);

//            std::cout << forbidden_pos_set.size() << " " << shrinked_pos_vect.size() << " " << free_pos_vect.size()
//                      << std::endl;
//            scenario_viewer(scenario);

        }
    }


}
