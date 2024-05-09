//
// Created by hiccup on 2020/5/15.
//

#pragma once


#include "../Param/TestingConfigTree.h"
#include "../Param/MT_Parameter.hpp"
#include "../Param/MR_Parameter.hpp"
#include "../API/IDataCollector.hpp"
#include "../API/IDataAnalyzer.hpp"

namespace TestingFramework {
    using IDataCollector_ptr = std::shared_ptr<IDataCollector>;
    using IDataAnalyzer_ptr = std::shared_ptr<IDataAnalyzer>;

    class IExecution {
    protected:
        virtual void testingCodeExecutionFunc(MR_TestCase_ptr case_ptr) = 0;

        virtual string originTestCaseResultReaderFunc(MR_TestCase_ptr case_ptr) = 0;

        virtual void lazyGenerateFollowUpTestCase(MR_TestUnit_ptr unit_ptr) = 0;

    public:

        IDataCollector_ptr data_collector_ptr;
        IDataAnalyzer_ptr data_analyzer_ptr;

        virtual void executeOriginCase(MR_TestUnit_ptr unit_ptr) = 0;

        virtual void executeFollowCases(MR_TestUnit_ptr unit_ptr) = 0;
    };
}