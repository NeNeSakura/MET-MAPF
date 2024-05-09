//
// Created by hiccup on 2020/5/15.
//

#pragma once

#include <vector>
#include <string>
#include "../../API/IDataAnalyzer.hpp"
#include "../../Param/TestingConfigTree.h"
#include "../MAPF_Point2D.hpp"
#include "MAPF_AnalysisBase.h"
#include "MAPF_AnalyzeParam.h"



namespace TestingFramework {

    namespace MAPF {
        class MAPF_DataAnalyzer : public IDataAnalyzer {
        protected:
            std::vector<string> group_result;

        public:
            string ArrangeAnalysisRecord(const MR_ConfigTree &tree) override;

            string ReadCaseResult(const MR_TestCase_ptr case_ptr) override;

            string UnitAnalysisRecording(const MR_TestUnit_ptr unit_ptr) override;

        protected:
            string ReadRelationGroupAnalyseRecord(const MR_RelationGroup_ptr group_ptr);

            string ReadUnitRecord(const MR_TestUnit_ptr unit_ptr);

            void ReadCaseResultPIBTV1(const MR_TestCase_ptr &case_ptr, const json &j, PathsVector &paths_vect);
//            void ReadCaseResultPIBTV2(MR_TestCase_ptr &case_ptr, const json &j, PathsVector &paths_vect);

            void ReadCaseResultPIBTV2(const MR_TestCase_ptr &case_ptr, const json &j, PathsVector &paths_vect);
        };
    }


}