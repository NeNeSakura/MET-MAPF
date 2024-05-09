//
// Created by hiccup on 2020/5/19.
//

#include "../../../include/MAPF/DataHandler/MAPF_AnalyzeParam.h"

namespace TestingFramework {
    namespace MAPF {

        double MAPF_AnalyzeParam::validation_threshold = 0.1;
        const long MAPF_AnalyzeParam::time_out_seconds_threshould;
        bool MAPF_AnalyzeParam::save_log = false;
        MAPF_AnalyzeParam::execution_t MAPF_AnalyzeParam::exec_type = MAPF_AnalyzeParam::CLI;
        std::string MAPF_AnalyzeParam::execution_path_GUI = "/home/hiccup/Projects/ZXY-work/pibt/bin/pibt";
        std::string MAPF_AnalyzeParam::execution_path_CLI = "/home/hiccup/Projects/ZXY-work/pibt/bin/testapp";
        std::string MAPF_AnalyzeParam::failed_output_template = "/home/hiccup/Projects/ZXY-work/MetamorphicTestingFramework/MT-Framework/error-output-template.txt";
    }


}
