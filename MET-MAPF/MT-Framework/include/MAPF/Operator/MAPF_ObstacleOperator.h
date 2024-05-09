//
// Created by hiccup on 2020/5/20.
//

#pragma once


#include <functional>
#include <cmath>
#include <iterator>     // std::advance
#include <boost/format/format_class.hpp>
#include <boost/geometry/geometry.hpp>
#include "../../API/IOperator.hpp"
#include "../../RandomHelper.h"
#include "../Relations/MAPF_RelationHelper.h"
#include "MAPF_Operator.h"
#include "MAPF_ObstacleParam.hpp"

namespace TestingFramework {
    namespace MAPF {


        class OperDecreaseObstacle : public IOperator {
        public:
            OperDecreaseObstacleParam param;

            OperDecreaseObstacle(const string &json_param_str) {
                operator_name = "Erosion Operation";
                operator_param = json_param_str;
                param = json::parse(json_param_str);

                //{"random":true , "erosion":false, "ratio" : 0.1}
                //if erosion == 1 then erosion algorithm for obstacle are executed
                //if random == 1 then random part of obstacle are deleted, else all satisfied obstacles are deleted.
                // ratio = [0,1]
            }

            void RelationOperation(string &testcase_param_json, const string &origincase_result = "") override;

        protected:

            std::set<FPoint2> GetErosionPos(const string &origincase_result, vector <FPoint2> &obstacle_pos_vect) const;
        };


        class OperIncreaseObstacle : public IOperator {
        public:
            OperInceaseObstacleParam param;

            OperIncreaseObstacle(const string &json_param_str) {
                operator_name = "Dilation Operation";
                operator_param = json_param_str;
                param = json::parse(json_param_str);

                //{"random":true , "dilation":false, "ratio" : 0.1, "critical" : false , "shrinked" : true}
                //if dilation == 1 then dilation algorithm for obstacle are executed
                //if random == 1 then random part of obstacle are deleted, else all satisfied obstacles are deleted.
                //ratio = [0,1]
            }

            void RelationOperation(string &testcase_param_json, const string &origincase_result = "") override;

        };
    }
}




