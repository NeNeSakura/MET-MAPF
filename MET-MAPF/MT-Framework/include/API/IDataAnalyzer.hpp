//
// Created by hiccup on 2020/5/15.
//

#pragma once

#include "IRelation.hpp"

namespace TestingFramework {
    class IDataAnalyzer {
    public:
        virtual string ReadCaseResult(const MR_TestCase_ptr case_ptr) = 0;

        virtual string ArrangeAnalysisRecord(const MR_ConfigTree &tree) = 0;

        virtual string UnitAnalysisRecording(const MR_TestUnit_ptr unit_ptr) =0;

        //建议在实现接口时补充的函数
        //protected:
        //virtual string ReadRelationGroupAnalyseRecord(MR_RelationGroup_ptr group_ptr) = 0;
    };


}