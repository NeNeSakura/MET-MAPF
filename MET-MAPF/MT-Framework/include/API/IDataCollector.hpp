//
// Created by hiccup on 2020/5/13.
//

#pragma once

#include <string>
#include "IRelation.hpp"

namespace TestingFramework {
    using std::string;
    using nlohmann::json;

    class IDataCollector {
    public:
        IDataCollector(const string &path = ""):target_folder_path(path){};
        string target_folder_path;

        virtual void DataCollectionPreprocessor(std::string const &param_raw_str, const MR_TestCase_ptr &case_ptr) =0;
        virtual void DataCollection(MR_ConfigTree &tree) = 0;
    };
}
