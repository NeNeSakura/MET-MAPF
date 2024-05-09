//
// Created by hiccup on 2020/8/11.
//

#pragma once

#include <nlohmann/json.hpp>
#include <vector>

namespace TestingFramework {
    namespace MAPF {
        using nlohmann::json;

        class OperPriorityExchangeParam {
        public:
            std::vector<int> change_list;
            double ratio = 0.1;
            int num = 1;            //follow case num, called in @initFollowCaseParamVector;
        };

        inline void to_json(json &j, const OperPriorityExchangeParam &p) {
            j = json{{"change_list", p.change_list},
                     {"ratio",p.ratio},
                     {"num",p.num}};
        }

        inline void from_json(const json &j, OperPriorityExchangeParam &p) {
            j.at("change_list").get_to(p.change_list);
            j.at("ratio").get_to(p.ratio);
            j.at("num").get_to(p.num);
        }

    }
}