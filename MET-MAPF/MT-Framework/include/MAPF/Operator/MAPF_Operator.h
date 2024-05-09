//
// Created by hiccup on 2020/5/5.
//

#pragma once

#include <functional>
#include <cmath>
#include <regex>
#include <string>
#include <vector>
#include <map>
#include <random>

#include <boost/format/format_class.hpp>
#include <boost/geometry/geometry.hpp>
#include <boost/core/swap.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include "easyloggingpp/easylogging++.h"
#include <nlohmann/json.hpp>

#include "../../API/IOperator.hpp"
#include "../../RandomHelper.h"
#include "../MAPF_Point2D.hpp"
#include "../../Param/MR_Parameter.hpp"
#include "../../Param/MT_Parameter.hpp"


namespace TestingFramework {
    namespace MAPF {

        using boost::format;


        //typedef boost::geometry::model::d2::point_xy<int> Point;

//        namespace bg = boost::geometry;
//        using point = bg::model::point<int, 2, bg::cs::cartesian>;

        struct MAPF_TaskItem {
//            Point2D<int> start;
//            Point2D<int> goal;

            FPoint2 start;
            FPoint2 goal;
            size_t idx;

//            MAPF_TaskItem(const MAPF_TaskItem& rhs) {
//                idx = rhs.idx;
//                start = rhs.start;
//                goal = rhs.goal;
//            }
//
//            MAPF_TaskItem() {
//                idx = 0;
//                start = FPoint2 (0,0);
//                goal = FPoint2 (0,0);
//            }
//
//            MAPF_TaskItem(const FPoint2& s,const FPoint2& g, size_t i) {
//                idx = i;
//                start = s;
//                goal = g;
//            }
//
//            ~MAPF_TaskItem(){
//
//            }

            inline bool operator ==(const MAPF_TaskItem &rhs) {
                if (start == rhs.start && goal == rhs.goal && idx == rhs.idx)
                    return true;
                else return false;
            }
        };

        inline void to_json(json &j, const MAPF_TaskItem &p) {
            j = json{{"start", p.start},
                     {"goal",  p.goal},
                     {"idx",   p.idx}};
        }

        inline void from_json(const json &j, MAPF_TaskItem &p) {
            j.at("start").get_to(p.start);
            j.at("goal").get_to(p.goal);
            j.at("idx").get_to(p.idx);
        }

        using MAPF_Task = vector<MAPF_TaskItem>;

        struct MAPF_Map {
            int width;
            int height;
            string type;
            std::vector<string> map_data;
        };

        struct MAPF_Scenario {
            MAPF_Map map_info;
            MAPF_Task task_info;
        };

        /// Multi-Agent-Path-Finding relation base;
        class MAPF_OperateFuncBase {
        public:
            std::function< const bool(Point2D<int>)
            >
            IsSafePoint;
        };

        void scenario_viewer(const MAPF_Scenario &scenario);


    }
}




