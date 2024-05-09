//
// Created by hiccup on 2020/5/5.
//

#include "../../../include/MAPF/Relations/MAPF_RelationHelper.h"
#include <boost/algorithm/string/trim.hpp>
#include <sstream>

namespace TestingFramework {
    namespace MAPF {

        std::map<string,vector<FPoint2>> MAPF_RelationHelper::map_cache_4_nonedge;
        std::map<string,MAPF_Map> MAPF_RelationHelper::map_cache_4_origin_map;

        //TODO wait for refactor
        MAPF_Map MAPF_RelationHelper::_MapReader(const string &map_str) {
            vector<string> data;
            std::string line;
            std::smatch results;
            std::regex r_type = std::regex(R"(^type\s(\w+))");
            std::regex r_height = std::regex(R"(^height\s(\d+))");
            std::regex r_width = std::regex(R"(^width\s(\d+))");
            std::regex r_map = std::regex(R"(^map)");

            std::stringstream ss(map_str);
            int w = -1;
            int h = -1;
            int j = 0;  // height
            string type = "";
            bool mapStart = false;

            using boost::split;
            using boost::token_compress_on;
            using boost::is_any_of;

            while (getline(ss, line)) {
                if (std::regex_search(line, results, r_height)) {
                    h = std::stoi(results[1].str());
                    //update: speed up the vector
                    data.reserve(h+1);
                }
                if (std::regex_search(line, results, r_width)) {
                    w = std::stoi(results[1].str());
                }
                if (type == "" && std::regex_search(line, results, r_type)) {
                    type = results[1].str();
                }
                if (mapStart) {
                    boost::trim_if(line, is_any_of("\t\n\v\f\r"));
                    // width check
                    if (line.size() != w) {
                        LOG(ERROR) << format{"@ScenarioReader: width is invalid, should be %1%"} % w;
                        std::exit(1);
                    }

                    data.push_back(line);
                    ++j;
                }

                if (std::regex_search(line, results, r_map)) mapStart = true;

            }


            // height check
            if (j != h) {
                LOG(ERROR) << format{"@ScenarioReader, height is invalid, should be %1% but actually %2%"} % h, j;
                std::exit(1);
            }

            return MAPF_Map{w, h, type, data};
        }

        std::string MAPF_RelationHelper::_MapWriter(MAPF_Map &map) {

            // data;
            std::stringstream ss;
            ss << format{"type %1%"} % map.type << endl;
            ss << format{"width %1%"} % map.width << endl;
            ss << format{"height %1%"} % map.height << endl;
            ss << "map" << endl;
            for (auto iter = map.map_data.begin(); iter != map.map_data.end(); iter++)
                ss << *iter << endl;

            return ss.str();
        }

        MAPF_Task MAPF_RelationHelper::_TaskReader(const string &task_str) {
            using nlohmann::json;
            MAPF_Task task;
            json array_items = json::parse(task_str);
            for (json &el : array_items) {
                MAPF_TaskItem item;
                item.idx = el.at("idx").get<unsigned>();
                item.start = el.at("start").get<FPoint2>();
                item.goal = el.at("goal").get<FPoint2>();
                task.push_back(item);
            }
            return task;
        }


        MAPF_Scenario
        MAPF_RelationHelper::ScenarioJsonReader(const string &MR_param_json_str) {
            json MR_param_json = json::parse(MR_param_json_str);
            return ScenarioJsonReader(MR_param_json);
        }


        MAPF_Scenario
        MAPF_RelationHelper::ScenarioJsonReader(const json &MR_param_json) {
            string map_str = MR_param_json.at("map").get<string>();
            string task_str = MR_param_json.at("task").get<string>();
            unsigned int seed =MR_param_json.at("seed").get<unsigned int>();
            auto mr_param = MR_param_json.at("mr_param").get<MR_Parameter>();
//            auto &origin_seed_list = mr_param.testunit_seeds;
            auto name = MR_param_json.at("origin_map_name").get<string>();
//            string origin_seed_list = mr_param.at("testunit_seeds").get<string>();
//            string name =j_mr_param.at("origin_map_name").get<string>();
//            if(origin_seed_list.find(seed) == string::npos)
            if(std::find(mr_param.testunit_seeds.begin(),mr_param.testunit_seeds.end(),seed)
                != mr_param.testunit_seeds.end())
            {
                //this is origin case
                if(map_cache_4_origin_map.find(name) == map_cache_4_origin_map.end())
                {
                    auto scenario = MAPF_RelationHelper::ScenarioReader(map_str, task_str);
                    map_cache_4_origin_map[name] = scenario.map_info;
                    return scenario;
                }
                else{
                    auto scenario = MAPF_Scenario{map_cache_4_origin_map[name], _TaskReader(task_str)};
                    return scenario;
                }
            }
            else{
                if(mr_param.reference_class_name == "RandomTesting")
                {
                    auto scenario = MAPF_Scenario{map_cache_4_origin_map[name], _TaskReader(task_str)};
                    return scenario;
                }
                else{
                    auto scenario = MAPF_RelationHelper::ScenarioReader(map_str, task_str);
                    return scenario;
                }
            }
        }

        void MAPF_RelationHelper::ScenarioJsonWriter(json &MR_param_json,
                                                     MAPF_Scenario &scenario) {
            auto result = MAPF_RelationHelper::ScenarioWriter(scenario);
            MR_param_json["map"] = result["map"];
            MR_param_json["task"] = result["task"];
            MR_param_json["agent_num"] = scenario.task_info.size();
        }

        void MAPF_RelationHelper::ScenarioJsonWriter(string &MR_param_json_str,
                                                     MAPF_Scenario &scenario) {
            json MR_param_json = json::parse(MR_param_json_str);
            auto result = MAPF_RelationHelper::ScenarioWriter(scenario);
            MR_param_json["map"] = result["map"];
            MR_param_json["task"] = result["task"];
            MR_param_json["agent_num"] = scenario.task_info.size();
            MR_param_json_str = MR_param_json.dump();
        }


        MAPF_Scenario
        MAPF_RelationHelper::ScenarioReader(const string &map_str, const string &task_str) {

            return MAPF_Scenario{_MapReader(map_str), _TaskReader(task_str)};
        }

        std::map<string, string>
        MAPF_RelationHelper::ScenarioWriter(MAPF_Scenario &scenario) {
            using std::pair;
            std::map<string, string> out;
            out.insert(pair<string, string>("map", _MapWriter(scenario.map_info)));
            out.insert(pair<string, string>("task", _TaskWriter(scenario.task_info)));

            return out;
        }

        std::string MAPF_RelationHelper::_TaskWriter(MAPF_Task &task) {

            json j = json::array();
            for (auto &item : task) {
                json o;
                o["idx"] = item.idx;
                o["start"] = item.start;
                o["goal"] = item.goal;
                j.push_back(o);
            }

//            return j.dump(4);
            return j.dump();
        }

        vector<std::pair<FPoint2, FPoint2>>
        MAPF_RelationHelper::getNonEdgeRandomStartGoal(unsigned num, std::mt19937 &MT) {


            vector<FPoint2> shrinks(starts.size());
            vector<FPoint2> dropouts;
            vector<FPoint2> shrank_pos_vect;

            //cache
            if (map_cache_4_nonedge.find(filename) == map_cache_4_nonedge.end()) {
                std::copy(starts.begin(), starts.end(), shrinks.begin());
                for (auto &p : shrinks) {
                    vector<FPoint2> tmp = {FPoint2{p.x - 1, p.y},
                                           FPoint2{p.x + 1, p.y},
                                           FPoint2{p.x, p.y - 1},
                                           FPoint2{p.x, p.y + 1}};
                    if (std::any_of(tmp.cbegin(),
                                    tmp.cend(),
                                    [&](const FPoint2 &a) -> bool {
                                        return (std::find(shrinks.begin(), shrinks.end(), a) == shrinks.end());
                                    })) {
                        dropouts.push_back(p);
                    }
                }

                std::sort(shrinks.begin(), shrinks.end());
                std::set_difference(shrinks.cbegin(), shrinks.cend(),
                                    dropouts.cbegin(), dropouts.cend(),
                                    std::back_inserter(shrank_pos_vect));


                VLOG(3) << format{"origin available nodes: %1%, shrinked nodes %2%,"
                                  " shrank_pos_vect nodes: %3%"}
                           % shrinks.size() % dropouts.size() % shrank_pos_vect.size();

                map_cache_4_nonedge.insert({filename, shrank_pos_vect});
            } else {
                shrank_pos_vect = map_cache_4_nonedge.at(filename);
            }


            if (num > shrank_pos_vect.size()) {
                LOG(WARNING) << format{"map: %1% has available shrank position number : %2%. "
                                 "However, input num is %3%, which is larger than positon number"}
                                 % filename % shrank_pos_vect.size() % num;
                num = std::round(num / (double) starts.size() * (double) shrank_pos_vect.size());
            }
            if (num == 0)
                num = 1;
            if (num > shrank_pos_vect.size()) {
                LOG(ERROR) << "error@Graph::getNonEdgeRandomStartGoal, over node size";
                std::exit(1);
            }

            vector<std::pair<FPoint2, FPoint2>> data;
            vector<FPoint2> ss(shrank_pos_vect.size());
            vector<FPoint2> gs(shrank_pos_vect.size());
            bool flg;

            std::copy(shrank_pos_vect.begin(), shrank_pos_vect.end(), ss.begin());
            std::copy(shrank_pos_vect.begin(), shrank_pos_vect.end(), gs.begin());

            while (true) {
                data.clear();
                std::shuffle(ss.begin(), ss.end(), MT);
                std::shuffle(gs.begin(), gs.end(), MT);

                flg = true;
                for (int i = 0; i < num; ++i) {
                    if (ss[i] != gs[i]) {
                        data.push_back({ss[i], gs[i]});
                    } else {
                        flg = false;
                        break;
                    }
                }

                if (flg) break;
            }

            return data;
        }

        vector<std::pair<FPoint2, FPoint2>>
        MAPF_RelationHelper::getRandomStartGoal(unsigned num, std::mt19937 &MT) {
            //TODO check correct ness.

            if (num > starts.size() || num > goals.size()) {
                LOG(ERROR) << "error@Graph::getStartGoal, over node size";
                std::exit(1);
            }

            vector<std::pair<FPoint2, FPoint2>> data;
            vector<FPoint2> ss(starts.size());
            vector<FPoint2> gs(goals.size());
            bool flg;

            std::copy(starts.begin(), starts.end(), ss.begin());
            std::copy(goals.begin(), goals.end(), gs.begin());

            while (true) {
                data.clear();
                std::shuffle(ss.begin(), ss.end(), MT);
                std::shuffle(gs.begin(), gs.end(), MT);

                flg = true;
                for (int i = 0; i < num; ++i) {
                    if (ss[i] != gs[i]) {
                        data.push_back({ss[i], gs[i]});
                    } else {
                        flg = false;
                        break;
                    }
                }

                if (flg) break;
            }

//            for(auto &item : data)
//            {
//                std::cout<< "s:" << item.first.print() << "t:"<< item.second.print() << std::endl;
//            }
            return data;
        }

        string MAPF_RelationHelper::getRandomStartGoal_RawJson(std::mt19937 &MT, unsigned int num) {
            using nlohmann::json;

            vector<std::pair<FPoint2, FPoint2>> data;
            if (!shrinked_random_mission)
                data = getRandomStartGoal(num, MT);
            else {
                data = getNonEdgeRandomStartGoal(num, MT);
            }
            json j;
            int i = 0;
            for (auto &item : data) {
                json o;
                o["idx"] = i;
                o["start"] = item.first;
                o["goal"] = item.second;
                j.push_back(o);
                i++;
            }
            return j.dump(4);
        }

        string MAPF_RelationHelper::getRandomStartGoal_RawJson(double ratio, std::mt19937 &MT) {
            if (ratio < 0) {
                LOG(ERROR)
                        << format{"@MAPF_RelationHelper.getRandomStartGoal_RawJson illegal ratio number %1%"} % ratio;
                return "";
            } else if (ratio < 1) {
                return getRandomStartGoal_RawJson(MT, (unsigned) std::round(ratio * starts.size()));
            } else
                return getRandomStartGoal_RawJson(MT, (unsigned) std::round(ratio));
        }

        void MAPF_RelationHelper::InitTaskData(std::string &filename) {

            using namespace boost::algorithm;

            // read file
            std::ifstream ss(filename);
            if (!ss) {
                LOG(ERROR) << "error@MAPF_RelationHelper::InitTaskData, file "
                           << filename
                           << " does not exist" << "\n";
                std::exit(1);
            }

            vector<FPoint2> data;
            std::string line;
            std::smatch results;

            int w = -1;
            int h = -1;
            int j = 0;  // height
            char s;
            string type = "";
            bool mapStart = false;

            std::regex r_type = std::regex(R"(^type\s(\w+))");
            std::regex r_height = std::regex(R"(^height\s(\d+))");
            std::regex r_width = std::regex(R"(^width\s(\d+))");
            std::regex r_map = std::regex(R"(^map)");

            // read fundamental graph params

            using boost::split;
            using boost::token_compress_on;
            using boost::is_any_of;
            while (getline(ss, line)) {
                if (std::regex_search(line, results, r_height)) {
                    h = std::stoi(results[1].str());
                }
                if (std::regex_search(line, results, r_width)) {
                    w = std::stoi(results[1].str());
                }
                if (type == "" && std::regex_search(line, results, r_type)) {
                    type = results[1].str();
                }
                if (mapStart) {
                    // width check
                    boost::trim_if(line, is_any_of("\t\n\v\f\r"));
                    if (line.size() != w) {
                        LOG(ERROR) << format{"@ScenarioReader: width is invalid, should be %1%"} % w;
                        std::exit(1);
                    }
                    for (int i = 0; i < w; ++i) {
                        s = line[i];
//                        id = j * w + i;
                        if (s == 'T' or s == '@') continue;
                        data.push_back(FPoint2(i, j));
                    }
                    ++j;
                }

                if (std::regex_search(line, results, r_map)) mapStart = true;
            }


            // height check
            if (j != h) {
                LOG(ERROR) << format{"@ScenarioReader, height is invalid, should be %1% but actually %2%"} % h, j;
                std::exit(1);
            }

            this->filename = filename;
            starts = vector<FPoint2>(data.begin(), data.end());
            goals = vector<FPoint2>(data.begin(), data.end());
        }

        string MAPF_RelationHelper::RawMapReader(std::string &filename) {
            // read file
            std::ifstream ss(filename);
            if (!ss) {
                LOG(ERROR) << "error@SimpleGrid::setBasicParams, file "
                           << filename
                           << " does not exist" << "\n";
                std::exit(1);
            }

            string line;
            string str;
            while (getline(ss, line)) {
                boost::trim_if(line, boost::is_any_of("\t\n\v\f\r"));
                str += line + "\n";
            }
            return str;
        }



    }



}
