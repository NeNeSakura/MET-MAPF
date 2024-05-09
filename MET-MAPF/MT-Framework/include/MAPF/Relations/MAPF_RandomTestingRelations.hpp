#pragma once

#include "MAPF_RelationBase.h"
#include "../DataHandler/MAPF_AnalysisBase.h"
#include "../Operator/MAPF_ObstacleOperator.h"

namespace TestingFramework {
    namespace MAPF {

        class RandomTestingOperation : public IOperator {
        public:
            RandomTestingOperation(const string &json_param_str) {
                operator_name = "RandomTesting Operation";
                operator_param = json_param_str;

                //{"random":true , "erosion":false, "ratio" : 0.1}
                //if erosion == 1 then erosion algorithm for obstacle are executed
                //if random == 1 then random part of obstacle are deleted, else all satisfied obstacles are deleted.
                // ratio = [0,1]
            }

            void RelationOperation(string &testcase_param_json, const string &origincase_result = "") override {

                json j_param = json::parse(operator_param);
                if(j_param["random_seed"])
                {
                    json j = json::parse(testcase_param_json);
                    j["seed"] = RandomHelper::GetRandomInt();       //change execution seed
                    testcase_param_json = j.dump();
                }
                return;//RandomTesting ,empty operation~
            }

        protected:

        };


        class RandomTesting : public MAPF_MetamorphicRelationBase {
        public:

            RandomTesting(const MR_Parameter &mr_param)
                    : MAPF_MetamorphicRelationBase(mr_param,
                            ObjPool::allocate<RandomTestingOperation>(mr_param.MR_para_str)) {
                //parameter list
                // random == 1 ?
                // ratio == [0,1] ?
            };


            bool isRelationSatisfied(const string &origin_result, const vector<string> &follow_results) override {
                //random testing do not need any relations
                return true;
            }


        public:

            vector<string>
            followCaseParamGenerator(string &origin_case_param, const string &origin_case_result) override {
                if (operator_ptr == NULL)
                    LOG(ERROR) << "@RandomTesting Class, operator_ptr is empty";

                return MAPF_MetamorphicRelationBase::followCaseParamGenerator(origin_case_param, origin_case_result);
            }

        };





    }
}