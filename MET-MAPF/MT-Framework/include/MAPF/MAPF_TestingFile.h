//
// Created by hiccup on 2020/7/28.
//

#pragma once

#include <vector>
#include <random>
#include <string>
#include <boost/date_time/posix_time/time_formatters.hpp>
#include <boost/date_time/time_clock.hpp>
#include <boost/filesystem.hpp>

#include "MAPF_Point2D.hpp"
#include "../Param/MR_Parameter.hpp"
#include "../Param/MT_Parameter.hpp"
#include "../API/IOperator.hpp"
#include "../API/IRelation.hpp"
#include "Operator/MAPF_Operator.h"
#include "Relations/MAPF_Relation.h"
#include "Relations/MAPF_RelationHelper.h"
#include "Relations/MAPF_ViolationCriteria.h"


#include "DataHandler/MAPF_AnalyzeParam.h"

namespace TestingFramework {

    namespace MAPF {
        using std::vector;
        using std::string;

        class MT_HyperPara {
        public:
            // hyper
            bool shrink_mission_pos = true; //true: do not generate any start/goal near the obstacle / map boundary.
            double ratio = 10;              //[0~1): agent num, generate ratio * total feasible pos in map;
            //[1~inf): set agent num to ratio;

            // algorithm
            //option: { PIBT, HCA, WHCA, PPS, CBS, ECBS, iECBS, TP, winPIBT }
            string algorithm = "PIBT";
            int overstep_limit = 600;
            int overtime_limit = 1000;
        };

        inline void to_json(json &j, const MT_HyperPara &p) {
            j = json{{"shrink_mission_pos", int(p.shrink_mission_pos)},
                     {"ratio",              p.ratio},
                     {"algorithm",          p.algorithm},
                     {"overstep_limit",     p.overstep_limit},
                     {"overtime_limit",     p.overtime_limit}};
        }

        inline void from_json(const json &j, MT_HyperPara &p) {
            p.shrink_mission_pos = j.at("shrink_mission_pos").get<int>();
            j.at("ratio").get_to(p.ratio);
            j.at("algorithm").get_to(p.algorithm);
            j.at("overstep_limit").get_to(p.overstep_limit);
            j.at("overtime_limit").get_to(p.overtime_limit);
        }


        class MAPF_TestingFile {

        public:
            MT_HyperPara para;

            size_t repeat_unit = 20;
            size_t repeat_case = 10;        //only random testing available.


            string data_path_root = "/home/hiccup/Append/ZXY-Tmp/RandomTestingData/";
            string case_path_root = "/home/hiccup/Projects/ZXY-work/pibt/map/";
            string tmp_file_path_root = "/home/hiccup/Append/ZXY-Tmp/";
            string test_program_root = "/home/hiccup/Projects/ZXY-work/pibt/";

            size_t idx;
            size_t total_num;

            std::vector<std::string> maps = {"arena.map", "warehouse_middle.map", "beautiful8f.map",
                    //"den001d.map",
                                             "den009d.map",
                    //"den011d.map",
                                             "den101d.map", "den201d.map",
                                             "den202d.map", "den204d.map", "den207d.map", "den312d.map",
                    //"den520d.map",
                                             "empty-32-32.map"};

            //===============================================//
            //execution parameters
            string GUI_path = "/home/hiccup/Projects/ZXY-work/pibt/bin/pibt";
            string CLI_path = "/home/hiccup/Projects/ZXY-work/pibt/bin/testapp";
            string log_config_filename = "my_log.conf";
            bool save_log = false;
            bool is_CLI = true;
            double validation_threshold = 0.1;
            ViolationCriteria::ThresholdTypeEnum validation_threshold_type = ViolationCriteria::ThresholdTypeEnum::RatioValue;
            string failed_output_template = "/home/hiccup/Projects/ZXY-work/MetamorphicTestingFramework/MT-Framework/error-output-template.txt";

            json RandomTestingFileGenerator(const vector<unsigned> &random_list);

            json MetamorphicTestingFileGenerator(std::vector<MR_Parameter> &param_vect);

        protected:

            void InitExecutionParameters() {
                MAPF_AnalyzeParam::execution_path_GUI = GUI_path;
                MAPF_AnalyzeParam::execution_path_CLI = CLI_path;
                MAPF_AnalyzeParam::save_log = save_log;
                MAPF_AnalyzeParam::validation_threshold = validation_threshold; //留着吧，以后没准能用上，没必要封住
                MAPF_AnalyzeParam::failed_output_template = failed_output_template;

                if (is_CLI)
                    MAPF_AnalyzeParam::exec_type = MAPF_AnalyzeParam::CLI;
                else
                    MAPF_AnalyzeParam::exec_type = MAPF_AnalyzeParam::GUI;

                //violation criteria init;
                ViolationCriteria::Threshold = validation_threshold;
                ViolationCriteria::ThresholdType = validation_threshold_type;
            }

            void InitBar() {
                ProgressBarHelper::single_instance.current_epoch = idx;
                ProgressBarHelper::single_instance.total_epoch = total_num;
                json j = para;
                ProgressBarHelper::single_instance.extra_paint_info = j.dump(4);
            }

            void DeleteFiles() {
                using namespace boost::filesystem;
                path dir_path(tmp_file_path_root);
                if (!exists(dir_path))
                {
                    create_directories(dir_path/"log");
                    return;
                }
                else {
                    for (directory_iterator iter(dir_path); iter != directory_iterator(); ++iter) {
                        std::string str = iter->path().filename().string();
                        if (is_regular_file(iter->status())) {
                            auto extension = iter->path().extension().string();
                            if (str.find("tsample-param") != string::npos && extension == ".txt") {
                                remove(iter->path());
                                LOG(INFO) << str << " [delete]"; // display filename only
                            } else if (str.find("tmp_task") != string::npos && extension == ".json") {
                                remove(iter->path());
                                LOG(INFO) << str << " [delete]"; // display filename only
                            } else if (str.find("tmp_map") != string::npos && extension == ".map") {
                                remove(iter->path());
                                LOG(INFO) << str << " [delete]"; // display filename only
                            } else {

                            }
                        }
                    }
                }
            }

            void LogInit() const {
                boost::filesystem::path base_path(tmp_file_path_root);
                auto log_path = base_path / log_config_filename;
                auto log_path_2 = base_path.parent_path() / log_config_filename;
                if (boost::filesystem::exists(log_path)) {
                    el::Configurations conf(log_path.string());
                    el::Loggers::reconfigureAllLoggers(conf);
                } else if (boost::filesystem::exists(log_path_2)) {
                    el::Configurations conf(log_path_2.string());
                    el::Loggers::reconfigureAllLoggers(conf);
                } else {
                    el::Configurations defaultConf;
                    defaultConf.setToDefault();
                    el::Loggers::reconfigureAllLoggers(defaultConf);
                }
            }

        public:
            void Init() {
                LogInit();
                InitExecutionParameters();
                InitBar();
                DeleteFiles();
            }


        };

    }
}



