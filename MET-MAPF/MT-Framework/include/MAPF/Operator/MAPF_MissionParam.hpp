//
// Created by hiccup on 2020/8/11.
//

#pragma once

#include <nlohmann/json.hpp>
#include <vector>
#include "MAPF_Operator.h"

namespace TestingFramework {
    namespace MAPF {
        using nlohmann::json;

        class OperExchangeParam {
        public:
            //considering random seed for algorithm execution, we set an parameter
            bool need_random_seed = false;
        };

        inline void to_json(json &j, const OperExchangeParam &p) {
            j = json{{"need_random_seed", p.need_random_seed}};
        }

        inline void from_json(const json &j, OperExchangeParam &p) {
            j.at("need_random_seed").get_to(p.need_random_seed);
        }


        class OperExchangeRandomParam {
        public:
            //random exchange start and target pos with "num" follow cases execution.
            int num = 1;            //follow case num, called in @initFollowCaseParamVector;
        };

        inline void to_json(json &j, const OperExchangeRandomParam &p) {
            j = json{{"num", p.num}};
        }

        inline void from_json(const json &j, OperExchangeRandomParam &p) {
            j.at("num").get_to(p.num);
        }


        class OperDeleteAgentParam {
        public:
            std::vector<size_t> delete_list;
            double ratio = 0.1;
            int num = 1;            //follow case num, called in @initFollowCaseParamVector;
        };

        inline void to_json(json &j, const OperDeleteAgentParam &p) {
            j = json{{"delete_list", p.delete_list},
                     {"ratio",       p.ratio},
                     {"num",         p.num}};
        }

        inline void from_json(const json &j, OperDeleteAgentParam &p) {
            j.at("delete_list").get_to(p.delete_list);
            j.at("ratio").get_to(p.ratio);
            j.at("num").get_to(p.num);
        }


        ///if change list is empty ,then random insert #agent * ratio new agents.
        ///else execute insert operation as insert_priority_list records.(ratio is invalid in this situation)
        class OperAddAgentParam {

        public:
            double ratio = 0.1;
            ///>follow case num, called in @initFollowCaseParamVector
            int num = 1;
            bool is_probe = true;
            ///> if prepare insert operation, record the insert index in origin task list.
            std::vector<size_t> insert_priority_list;
            ///> if need to specify selected position. use this parameter;
            std::vector<MAPF_TaskItem> insert_item_list;
            ///> if is_probe == true && random == true, random pos generated probe
            bool is_non_window_probe = true;
            /// if is_probe == true && random == false,
            /// pos will init near any agent with specific window size as the boundary
            size_t window_size = 3;
        };


        inline void to_json(json &j, const OperAddAgentParam &p) {
            j = json{{"insert_priority_list", p.insert_priority_list},
                     {"ratio",                p.ratio},
                     {"num",                  p.num},
                     {"is_probe",             p.is_probe},
                     {"insert_item_list",     p.insert_item_list},
                     {"is_non_window_probe",  p.is_non_window_probe},
                     {"window_size",          p.window_size}};
        }

        inline void from_json(const json &j, OperAddAgentParam &p) {
            j.at("insert_priority_list").get_to(p.insert_priority_list);
            j.at("ratio").get_to(p.ratio);
            j.at("num").get_to(p.num);
            j.at("is_probe").get_to(p.is_probe);
            j.at("insert_item_list").get_to(p.insert_item_list);
            j.at("is_non_window_probe").get_to(p.is_non_window_probe);
            j.at("window_size").get_to(p.window_size);
        }


        class OperMoveAgentParam {
        public:
            enum POS_TYPE {
                START,
                TARGET
            };

        public:
            //std::set<size_t> move_distance_set;
            double ratio = 0.5;     // the min distance_func of path * ratio as the upperbound random move distance_func.

            int num = 1;            // follow case num, called in @initFollowCaseParamVector;
            // priority len(move_distance_set) > num
            POS_TYPE move_pos_type = START;
        };

        inline void to_json(json &j, const OperMoveAgentParam &p) {
            j = json{{"ratio",         p.ratio},
                     {"num",           p.num},
                     {"move_pos_type", p.move_pos_type}};
        }

        inline void from_json(const json &j, OperMoveAgentParam &p) {
            j.at("ratio").get_to(p.ratio);
            j.at("num").get_to(p.num);
            j.at("move_pos_type").get_to(p.move_pos_type);
        }
    };
}