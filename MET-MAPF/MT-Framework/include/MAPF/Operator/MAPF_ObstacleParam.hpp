//
// Created by hiccup on 2020/8/11.
//

#pragma once

#include <nlohmann/json.hpp>
#include <vector>

namespace TestingFramework {
    namespace MAPF {
        using nlohmann::json;


        class OperDecreaseObstacleParam {
        public:
            bool random = true;
            bool erosion = false;
            double ratio = 0.1;
            int num = 1;            //follow case num, called in @initFollowCaseParamVector;
        };

        inline void to_json(json &j, const OperDecreaseObstacleParam &p) {
            j = json{{"random", p.random},
                     {"erosion", p.erosion},
                     {"ratio",p.ratio},
                     {"num",p.num}};
        }

        inline void from_json(const json &j, OperDecreaseObstacleParam &p) {
            j.at("random").get_to(p.random);
            j.at("erosion").get_to(p.erosion);
            j.at("ratio").get_to(p.ratio);
            j.at("num").get_to(p.num);
        }



        class OperInceaseObstacleParam {
        public :
            bool random = true;
            bool dilation = false;
            bool critical = false;
            bool shrinked = true;
            double ratio = 0.1;
            int num = 1;//follow case num, called in @initFollowCaseParamVector;
        };

        inline void to_json(json &j, const OperInceaseObstacleParam &p) {
            j = json{{"random", p.random},
                     {"dilation", p.dilation},
                     {"critical", p.critical},
                     {"shrinked", p.shrinked},
                     {"ratio",p.ratio},
                     {"num",p.num}};
        }

        inline void from_json(const json &j, OperInceaseObstacleParam &p) {
            j.at("random").get_to(p.random);
            j.at("dilation").get_to(p.dilation);
            j.at("critical").get_to(p.critical);
            j.at("shrinked").get_to(p.shrinked);
            j.at("ratio").get_to(p.ratio);
            j.at("num").get_to(p.num);
        }
    };


}