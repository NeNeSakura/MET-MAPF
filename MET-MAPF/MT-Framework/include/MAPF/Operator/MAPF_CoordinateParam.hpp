//
// Created by hiccup on 2020/8/11.
//

#pragma once

#include <nlohmann/json.hpp>

namespace TestingFramework {
    namespace MAPF {
        using nlohmann::json;

        class OperRotationParam {
        public:
            double angle = 0;
        };

        inline void to_json(json &j, const OperRotationParam &p) {
            j = json{{"angle", p.angle}};
        }

        inline void from_json(const json &j, OperRotationParam &p) {
            j.at("angle").get_to(p.angle);
        }



        class OperMirrorParam
        {
        public:
            enum direction_t {
                horizon,
                vertical,
                center,
            };

            direction_t direction = horizon;
        };

        inline void to_json(json &j, const OperMirrorParam &p) {
            j = json{{"direction", p.direction}};
        }

        inline void from_json(const json &j, OperMirrorParam &p) {
            j.at("direction").get_to(p.direction);
        }
    }
}