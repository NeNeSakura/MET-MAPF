//
// Created by hiccup on 2020/5/14.
//

#pragma once

#include <functional>
#include <cmath>
#include <boost/format/format_class.hpp>
#include <boost/geometry/geometry.hpp>
#include <boost/core/swap.hpp>
#include "../../API/IOperator.hpp"
#include "../../RandomHelper.h"
#include "../Relations/MAPF_RelationHelper.h"
#include "MAPF_Operator.h"
#include "MAPF_CoordinateParam.hpp"

namespace TestingFramework {
    namespace MAPF {




        class OperRotation : public IOperator {

        public:
            OperRotationParam param;

            OperRotation(const string &json_param_str) {
                operator_name = "Rotation_MR3-2";
                operator_param = json_param_str;
                try{
                    param = json::parse(json_param_str);
                }
                catch (std::exception e) {
                    LOG(ERROR) << e.what();
                    LOG(ERROR) << boost::format{"@OperRotation: RelationOperation construction Failed,please Check!"};
                }
            }

            void RelationOperation(string &testcase_param_json, const string &origincase_result = "") override;

        protected:

            void rotate(MAPF_Scenario &scenario);
        };

        class OperMirror : public IOperator {
        public:
            OperMirrorParam param;
            OperMirror(const string &json_param_str) {
                operator_name = "Mirror_MR3-3";
                operator_param = json_param_str;
                try{
                    param = json::parse(json_param_str);
                }
                catch(std::exception e)
                {
                    LOG(ERROR) << e.what();
                    LOG(ERROR) << "@OperMirror RelationOperation construction failed";
                }
            }

            void RelationOperation(string &testcase_param_json, const string &origincase_result = "") override;

        protected:
            //void MirrorHorizon(MAPF_Map &map);

            void MirrorHorizon(MAPF_Scenario &scenario);

            //void MirrorVertical(MAPF_Map &map);

            void MirrorVertical(MAPF_Scenario &scenario);

        };

    }
}