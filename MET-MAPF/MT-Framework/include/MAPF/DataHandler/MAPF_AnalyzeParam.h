//
// Created by hiccup on 2020/5/19.
//

#pragma once

#include <string>

namespace TestingFramework {
    namespace MAPF {
        class MAPF_AnalyzeParam {
        public:
            enum execution_t{
                CLI,
                GUI
            };

            static double validation_threshold;
            constexpr static long time_out_seconds_threshould = 10;
            static bool save_log;
            static std::string execution_path_CLI;
            static std::string execution_path_GUI;
            static execution_t exec_type;
            static std::string failed_output_template;


        };
    }
}