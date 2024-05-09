//
// Created by hiccup on 2020/5/5.
//
#pragma once

#include <regex>
#include <string>
#include <vector>
#include <map>
#include  <random>

#include <boost/format/format_class.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/geometry/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <nlohmann/json.hpp>
#include "easyloggingpp/easylogging++.h"


#include "../Operator/MAPF_Operator.h"
#include "../MAPF_Point2D.hpp"
#include "../../Param/MR_Parameter.hpp"
#include "../../Param/MT_Parameter.hpp"

namespace TestingFramework {
    namespace MAPF {
        using std::string;
        using std::vector;
        using std::endl;
        using nlohmann::json;
        using boost::format;


        class MAPF_RelationHelper {
        private:
            vector<FPoint2> starts;
            vector<FPoint2> goals;
            bool shrinked_random_mission;
            static std::map<string,vector<FPoint2>> map_cache_4_nonedge;
            static std::map<string,MAPF_Map> map_cache_4_origin_map;
            string filename;
        protected:
            vector<std::pair<FPoint2, FPoint2>> getRandomStartGoal(unsigned num,
                                                                   std::mt19937 &MT);

            vector<std::pair<FPoint2, FPoint2>> getNonEdgeRandomStartGoal(unsigned num,
                                                                          std::mt19937 &MT);

            string getRandomStartGoal_RawJson(std::mt19937 &MT, unsigned int num);

        public:
            MAPF_RelationHelper(bool shrinked = false):shrinked_random_mission(shrinked){};

            string getRandomStartGoal_RawJson(double ratio, std::mt19937 &MT);

            //init starts and goals
            void InitTaskData(std::string &filename);

            string RawMapReader(std::string &filename);

            vector<string> RawMapStringReader(std::string map_data_str);



        private:
            static MAPF_Map _MapReader(const string &map_str);

            static string _MapWriter(MAPF_Map &map);

            static MAPF_Task _TaskReader(const string &task_str);

            static string _TaskWriter(MAPF_Task &task);

        public:

            static MAPF_Scenario ScenarioJsonReader(const json &MR_param_json);

            static void ScenarioJsonWriter(json &MR_param_json, MAPF_Scenario &scenario);

            static MAPF_Scenario ScenarioReader(const string &map_str, const string &task_str);

            static std::map<string, string> ScenarioWriter(MAPF_Scenario &scenario);

            static MAPF_Scenario ScenarioJsonReader(const string &MR_param_json_str);

            static void ScenarioJsonWriter(string &MR_param_json_str, MAPF_Scenario &scenario);

            bool CheckPointSafe(FPoint2 pos);
        };
    }
}
