//
// Created by hiccup on 2020/5/9.
//

#pragma once

#include <string>

#include <boost/date_time/posix_time/posix_time.hpp>
#include "../../Param/TestingConfigTree.h"
#include "../../Param/MT_Parameter.hpp"
#include "../../Param/MR_Parameter.hpp"
#include "../../API/IDataCollector.hpp"
#include "../../API/IDataAnalyzer.hpp"
#include "../../API/IExecution.hpp"

namespace TestingFramework {

    using IDataCollector_ptr = std::shared_ptr<IDataCollector>;
    using IDataAnalyzer_ptr = std::shared_ptr<IDataAnalyzer>;

    class MAPF_ExecutionBase: public IExecution {
    protected:
        virtual void testingCodeExecutionFunc(MR_TestCase_ptr case_ptr) = 0;

        virtual string originTestCaseResultReaderFunc(MR_TestCase_ptr case_ptr);

        void
        DataCollectionPreprocessor(const MR_TestCase_ptr &case_ptr,
                                   const boost::posix_time::ptime &case_start_time,
                                   const boost::posix_time::ptime &case_end_time);

        virtual void lazyGenerateFollowUpTestCase(MR_TestUnit_ptr unit_ptr);

    public:

        IDataCollector_ptr data_collector_ptr;
        IDataAnalyzer_ptr data_analyzer_ptr;

        virtual void executeOriginCase(MR_TestUnit_ptr unit_ptr);

        virtual void executeFollowCases(MR_TestUnit_ptr unit_ptr);

        bool CheckExcecutionLegal(MR_TestCase_ptr case_ptr);
    };


}