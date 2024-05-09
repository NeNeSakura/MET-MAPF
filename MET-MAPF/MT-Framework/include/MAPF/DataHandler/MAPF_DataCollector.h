//
// Created by hiccup on 2020/5/14.
//

#pragma once

#include "../../API/IDataCollector.hpp"
#include "MAPF_AnalysisBase.h"

namespace TestingFramework {
    namespace MAPF {
        class MAPF_DataCollector : public IDataCollector {

        public:
            MAPF_DataCollector(const string &path = "");

            void DataCollectionPreprocessor(std::string const &param_raw_str, const MR_TestCase_ptr &case_ptr) override;

            void DataCollection(MR_ConfigTree &root) override;

        protected:
            void ConstructDataMapJsonFile(const MR_ConfigTree &root) const;

            void ConstrcutFailedCaseJsonFile(const MR_ConfigTree &root);

            long time_out_threshould; //milliseconds;

            MR_UnitStatics_MAP unit_statics_map;

            void ZipDatas(MR_ConfigTree &tree);
        };
    }

}


