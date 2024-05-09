//
// Created by hiccup on 2020/5/14.
//

#include "../../../include/MAPF/Operator/MAPF_CoordinateOperator.h"


namespace TestingFramework {
    namespace MAPF {
        void OperRotation::rotate(MAPF_Scenario &scenario) {

            MAPF_Map &map = scenario.map_info;
            std::vector<string> new_data;
            auto &data = map.map_data;
            //FPoint2 center{(map.width - 1) / 2.0, (map.height - 1) / 2.0};
            FPoint2 center{0, 0};
            FPoint2 trans_origin = FPoint2(0, map.height - 1);
            trans_origin.rotate(-M_PI_2, center);

            //Task part
            MAPF_Task &task = scenario.task_info;

//            auto tmp = data;
//            auto item = task[0];
//                if(tmp[item.start.y][item.start.x] == 'T')
//                    tmp[item.start.y][item.start.x] = 'X';
//                else
//                    tmp[item.start.y][item.start.x] = 's';
//                if(tmp[item.goal.y][item.goal.x] == 'T')
//                    tmp[item.goal.y][item.goal.x] = 'X';
//                else
//                    tmp[item.goal.y][item.goal.x] = 'g';
//
//            for(auto &line : tmp)
//                std::cout<<line<<std::endl;



            for (auto &item :task) {
                item.start = item.start.rotate(-M_PI_2, center) - trans_origin;
                item.goal = item.goal.rotate(-M_PI_2, center) - trans_origin;
            }


            // map part
            for (int j = 0; j < map.width; ++j) {
                std::vector<char> new_line;
                for (int i = map.height - 1; i >= 0; --i)
                    new_line.push_back(data[i][j]);
                new_data.push_back(string(new_line.begin(), new_line.end()));
            }

            map.map_data = new_data;
            map.height = new_data.size();
            map.width = new_data.begin()->size();

//             tmp = new_data;
//             item = task[0];
//                if(tmp[item.start.y][item.start.x] == 'T')
//                    tmp[item.start.y][item.start.x] = 'X';
//                else
//                    tmp[item.start.y][item.start.x] = 's';
//                if(tmp[item.goal.y][item.goal.x] == 'T')
//                    tmp[item.goal.y][item.goal.x] = 'X';
//                else
//                    tmp[item.goal.y][item.goal.x] = 'g';
//
//            for(auto &line : tmp)
//                std::cout<<line<<std::endl;


        }

        void OperRotation::RelationOperation(string &testcase_param_json, const string &origincase_result) {

            //auto scenario = MAPF_RelationHelper::ScenarioJsonReader(testcase_param_json);
            try {
                //json j_param = json::parse(operator_param);
                auto scenario = MAPF_RelationHelper::ScenarioJsonReader(testcase_param_json);
                //double angle = j_param.at("angle").get<double>();
                double angle = param.angle;
                int rotate_num = (int) round(angle / M_PI_2) % 4;

                for (int i = 0; i < rotate_num; ++i)
                    rotate(scenario);
                MAPF_RelationHelper::ScenarioJsonWriter(testcase_param_json, scenario);
            }
            catch (std::exception e) {
                LOG(ERROR) << e.what();
                LOG(ERROR) << boost::format{"@OperRotation: RelationOperation function Failed,please Check!"};
            }

        }

        void OperMirror::RelationOperation(string &testcase_param_json, const string &origincase_result) {
            auto scenario = MAPF_RelationHelper::ScenarioJsonReader(testcase_param_json);
            //json j_param = json::parse(operator_param);
            //string mirror_type = j_param.at("direction").get<string>();
            string mirror_type = "NONE";

            if (param.direction == param.horizon) {
                mirror_type = "horizon";
                MirrorHorizon(scenario);
            } else if (param.direction == param.vertical) {
                mirror_type = "vertical";
                MirrorVertical(scenario);
            } else if (param.direction == param.center) {
                mirror_type = "center";
                MirrorHorizon(scenario);
                MirrorVertical(scenario);
            }
            else {
                LOG(ERROR) << "@OperMirror RelationOperation : operator_param[\"direction\"] type error:"
                           << boost::format{"No such type: %1%"} % mirror_type;
            }

            MAPF_RelationHelper::ScenarioJsonWriter(testcase_param_json, scenario);
        }


        void OperMirror::MirrorVertical(MAPF_Scenario &scenario) {

            auto &map = scenario.map_info;
            std::vector<string> new_data;
            auto &data = map.map_data;
            for (int i = 0; i < map.height; ++i) {
                std::vector<char> new_line;
                for (int j = map.width - 1; j >= 0; --j)
                    new_line.push_back(data[i][j]);
                new_data.push_back(string(new_line.begin(), new_line.end()));
            }
            map.map_data = new_data;

            auto &task = scenario.task_info;
            for (auto &item : task) {
                item.start.x = scenario.map_info.width - item.start.x - 1;
                item.goal.x = scenario.map_info.width - item.goal.x - 1;
            }

        }


        void OperMirror::MirrorHorizon(MAPF_Scenario &scenario) {

            auto &map = scenario.map_info;
            std::vector<string> new_data;
            auto &data = map.map_data;
            for (int i = map.height - 1; i >= 0; --i) {
                std::vector<char> new_line;
                for (int j = 0; j < map.width; ++j)
                    new_line.push_back(data[i][j]);
                new_data.push_back(string(new_line.begin(), new_line.end()));
            }
            map.map_data = new_data;

            auto &task = scenario.task_info;
            for (auto &item : task) {
                item.start.y = scenario.map_info.height - item.start.y - 1;
                item.goal.y = scenario.map_info.height - item.goal.y - 1;
            }

        }


    }

}