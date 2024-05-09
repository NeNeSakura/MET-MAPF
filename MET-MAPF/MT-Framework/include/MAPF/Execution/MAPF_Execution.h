//
// Created by hiccup on 2020/5/9.
//

#pragma once

#include "MAPF_ExecutionBase.h"

#include "../MAPF_Point2D.hpp"
#include "../Operator/MAPF_Operator.h"
#include "../DataHandler/MAPF_DataCollector.h"
#include "../DataHandler/MAPF_DataAnalyzer.h"
#include "../Relations/MAPF_RelationHelper.h"
#include "../../../../mem_pool/include/ObjPool.hpp"


namespace TestingFramework {
    namespace MAPF {


        class MAPF_Execution : public MAPF_ExecutionBase {
        public:
            MAPF_Execution(const std::string & data_path_root = "") {
                data_collector_ptr = ObjPool::allocate<MAPF_DataCollector>(data_path_root);
                data_analyzer_ptr = ObjPool::allocate<MAPF_DataAnalyzer>();
            }

            void executeOriginCase(MR_TestUnit_ptr unit_ptr) override;

            void executeFollowCases(MR_TestUnit_ptr unit_ptr) override;

            void lazyGenerateFollowUpTestCase(MR_TestUnit_ptr unit_ptr) override;

        protected:

            void testingCodeExecutionFunc(MR_TestCase_ptr case_ptr) override;

            string originTestCaseResultReaderFunc(MR_TestCase_ptr case_ptr) override;


        protected:

            bool FileExist(const string &filename) const;

            bool isScenarioLegal(const json &j) const;

            // for log file name
            string setCurrentTime();
        };


    }
}