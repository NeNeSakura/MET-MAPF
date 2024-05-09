//
// Created by hiccup on 2020/5/14.
//

#pragma once

#include <memory>
#include <string>

#include "boost/format.hpp"
#include "nlohmann/json.hpp"
#include "../Operator/MAPF_Operator.h"
#include "../MAPF_Point2D.hpp"
#include "../../API/IRelation.hpp"
#include "../../API/IOperator.hpp"
#include "../../Param/TestingConfigTree.h"
#include "../../RandomHelper.h"
#include "../../Param/MT_Parameter.hpp"
#include "MAPF_RelationBase.h"
#include "../Execution/MAPF_Execution.h"
#include "../Operator/MAPF_CoordinateOperator.h"
#include "../DataHandler/MAPF_AnalysisBase.h"

namespace TestingFramework {
    namespace MAPF {
        using nlohmann::json;
        using std::shared_ptr;
        using MT_Relation_ptr = shared_ptr<IMetamorphicRelation>;

        class Rotation : public MAPF_MetamorphicRelationBase {
        public:

            Rotation(const MR_Parameter &mr_param)
                    : MAPF_MetamorphicRelationBase(mr_param, ObjPool::allocate<OperRotation>(mr_param.MR_para_str)) {
                //parameter list
                //{angle:M_PI_2}
                violation_criteria_func = ViolationCriteria::Equal<double>;
            };


            bool isRelationSatisfied(const string &origin_result, const vector<string> &follow_results) override {
                RelationAnalyzeInfo info(origin_result, follow_results);

                vector<double> lengths;
                lengths.push_back(RelationAnalyzeInfo::GetLength(info.origin));
                for (auto &tmp : info.follows)
                    lengths.push_back(RelationAnalyzeInfo::GetLength(tmp));

//                return std::all_of(lengths.begin() + 1, lengths.end(),
//                                   [&](auto i) {
//                                       return i > lengths[0] * (1 - info.threshold) &&
//                                              i < lengths[0] * (1 + info.threshold);
//                                   });
                return violation_criteria_func(lengths, info.threshold);

            }


        public:

            vector<string>
            followCaseParamGenerator(string &origin_case_param, const string &origin_case_result) override {
                if (operator_ptr == NULL)
                    LOG(ERROR) << "@Rotation Class, operator_ptr is empty";

                return MAPF_MetamorphicRelationBase::followCaseParamGenerator(origin_case_param, origin_case_result);
            }

        };

        class Mirror : public MAPF_MetamorphicRelationBase {
        public:

            Mirror(const MR_Parameter &mr_param)
                    : MAPF_MetamorphicRelationBase(mr_param, ObjPool::allocate<OperMirror>(mr_param.MR_para_str)) {
                //parameter list
                //{direction:horizon} (horizon vertical)
                violation_criteria_func = ViolationCriteria::Equal<double>;
            };


            bool isRelationSatisfied(const string &origin_result, const vector<string> &follow_results) override {


                RelationAnalyzeInfo info(origin_result, follow_results);

                vector<double> lengths;
                lengths.push_back(RelationAnalyzeInfo::GetLength(info.origin));
                for (auto &tmp : info.follows)
                    lengths.push_back(RelationAnalyzeInfo::GetLength(tmp));

//                return std::all_of(lengths.begin() + 1, lengths.end(),
//                                   [&](auto i) {
//                                       return i > lengths[0] * (1 - info.threshold) &&
//                                              i < lengths[0] * (1 + info.threshold);
//                                   });
                return violation_criteria_func(lengths, info.threshold);

            }

        public:

            vector<string>
            followCaseParamGenerator(string &origin_case_param, const string &origin_case_result) override {

                if (operator_ptr == NULL)
                    LOG(ERROR) << "@Mirror Class, operator_ptr is empty";
                return MAPF_MetamorphicRelationBase::followCaseParamGenerator(origin_case_param, origin_case_result);


            }
        };

    }
}