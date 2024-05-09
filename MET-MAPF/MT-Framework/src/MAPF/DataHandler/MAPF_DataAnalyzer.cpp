//
// Created by hiccup on 2020/5/15.
//

#include "../../../include/MAPF/DataHandler/MAPF_DataAnalyzer.h"
//#include "../../../include/MAPF/DataHandler/MAPF_AnalysisBase.h"
//#include "../../../include/MAPF/DataHandler/MAPF_AnalyzeParam.h"
#include <string>
#include <boost/filesystem.hpp>
#include <regex>

namespace TestingFramework {

    using std::string;

    string MAPF::MAPF_DataAnalyzer::ReadRelationGroupAnalyseRecord(const MR_RelationGroup_ptr group_ptr) {

        VLOG(2) << format{"Read Analyzing Group Record, uuid = %1%"} % group_ptr->uuid_idx;

        for (auto &unit_ptr : group_ptr->unit_list) {
            ReadUnitRecord(unit_ptr);
        }

        //生成需要的 json_str
        return string();
    }

    string MAPF::MAPF_DataAnalyzer::ReadUnitRecord(const MR_TestUnit_ptr unit_ptr) {
        VLOG(2) << format{"Read Analyzing Unit Record, uuid = %1%"} % unit_ptr->uuid_idx;
        return "";

    }

    string MAPF::MAPF_DataAnalyzer::UnitAnalysisRecording(const MR_TestUnit_ptr unit_ptr) {

        VLOG(2) << format{"Start Analyzing Unit, uuid = %1%"} % unit_ptr->uuid_idx;
        vector<string> follow_up_results;
        for (auto unit : unit_ptr->follow_cases_list)
            follow_up_results.push_back(ReadCaseResult(unit));

        bool b = unit_ptr->relation_ptr->isRelationSatisfied(unit_ptr->origin_result, follow_up_results);

        auto statistics = MAPF::RelationAnalyzeInfo::getUnitStatics(unit_ptr, follow_up_results);

        //delete origin log files ,they are toooooooooo huge !!!
        //Give back the disk storage space to me!!!
        if (!MAPF::MAPF_AnalyzeParam::save_log) {
            namespace fs = boost::filesystem;
            {
                json j = json::parse(unit_ptr->origin_case->output_args);
                if(j.contains("raw_data_path"))
                    fs::remove(j["raw_data_path"].get<string>());
                else
                    LOG(WARNING) << "The SUT is terminated. No raw_data_path.";
            }
            for (auto & follow_case : unit_ptr->follow_cases_list)
            {
                json j = json::parse(follow_case->output_args);
                if(j.contains("raw_data_path"))
                    fs::remove(j["raw_data_path"].get<string>());
                else
                    LOG(WARNING) << "The SUT is terminated. No raw_data_path.";
            }
        }

        //save
        {
            json j;
            json j_array = statistics;
            j["pass"] = b;
            j["statistics"] = statistics;
            //生成需要的 json_str
            return j.dump();
        }
    }


    string MAPF::MAPF_DataAnalyzer::ArrangeAnalysisRecord(const MR_ConfigTree &tree) {
        LOG(INFO) << format{"Read analyzing record"};
        for (auto &group_ptr : tree.group_list) {
            ReadRelationGroupAnalyseRecord(group_ptr);
        }
        return "";
    }

    string MAPF::MAPF_DataAnalyzer::ReadCaseResult(const MR_TestCase_ptr case_ptr) {
        namespace fs = boost::filesystem;
        using fs::path;
        using std::regex;
        using std::smatch;
        using std::regex_search;
        using std::regex_match;

        json j = json::parse(case_ptr->output_args);
        std::cout<<case_ptr->output_args;
        PathsVector paths_vect;

#ifdef _SUT_VERSION_
        if (_SUT_VERSION_ == "2.0")
            ReadCaseResultPIBTV2(case_ptr, j, paths_vect);
        else
            ReadCaseResultPIBTV1(case_ptr, j, paths_vect);
#else
        ReadCaseResultPIBTV1(case_ptr, j, paths_vect);
#endif

        json j_output = paths_vect;
        return j_output.dump();
    }

    void MAPF::MAPF_DataAnalyzer::ReadCaseResultPIBTV2(
            const MR_TestCase_ptr &case_ptr, const json &j, PathsVector &paths_vect){
        using std::__cxx11::regex;
        using std::__cxx11::smatch;
//        std::cout<< case_ptr->input_args;
        string raw_data_path = "";
        if(j.contains("raw_data_path"))
            raw_data_path = j.at("raw_data_path").get<std::string>();

        boost::filesystem::path log_file_path = boost::filesystem::path(raw_data_path);
        if (!boost::filesystem::exists( log_file_path ))
        {
            LOG(WARNING)
                    << boost::format{"Problem occurs for analysis. test case uuid : %1%.\n "
                                     "No raw_data output. \n"
                                     "Create result based on the error-output-template\n"} % case_ptr->uuid_idx;

//            boost::filesystem::copy_file(TestingFramework::MAPF::MAPF_AnalyzeParam::failed_output_template, log_file_path);
            json j_in = json::parse(case_ptr->input_args);
            auto agent_size = j_in["agent_num"].get<int>();
//
//            auto agent_size = 1;
            for(int idx =0 ; idx < agent_size; ++idx )
            {
                TestingFramework::MAPF::PathInfo info;
                info.idx = idx;
                auto len =  j_in["overstep_limit"].get<int>();
                for(int i =0 ; i< len; i++)
                    info.path.push_back(FPoint2{0,0});
                paths_vect.push_back(info);
            }

            return;
        }

        regex r_agent_num(R"(agents=(\d+)$)");
        regex r_solved(R"(solved=(\d+)$)");
        regex r_path_coordinate(R"(\((-?\d+)?\s?,\s?(-?\d+)?\))");
        regex r_path_prefix(R"(solution=)");
        //        regex r_solved(R"^solved=(\d+)");
        //        r_soc = re.compile(r"soc=(\d+)")
        //        r_lb_soc = re.compile(r"lb_soc=(\d+)")
        //        r_makespan = re.compile(r"makespan=(\d+)")
        //        r_lb_makespan = re.compile(r"lb_makespan=(\d+)")
        //        r_comp_time = re.compile(r"comp_time=(\d+)")
        std::__cxx11::smatch match;

        try {
            std::ifstream ss(log_file_path.string());
            std::string line;
            while (getline(ss, line)) {
                if (regex_search(line, match, r_agent_num)) {
                    if (match.size() != 2)
                        LOG(WARNING) << boost::format{"@ReadCaseResult, test case uuid: %1%, log file format conflicts."
                                                      "no id info"} % case_ptr->uuid_idx;
                    else
                    {
                        for (auto i=0 ; i<std::stoi(match[1].str()); ++i)
                        {
                            TestingFramework::MAPF::PathInfo info;
                            info.idx = i;
                            paths_vect.push_back(info);
                        }
                    }
                }
                if(regex_search(line,match,r_solved)){
                    case_ptr->is_legal_execution = stoi( match[1].str());
                }

                if (regex_search(line, match, r_path_prefix))
                {
                    while(getline(ss, line))   //get path
                    {
                        int idx=0;
                        while (regex_search(line, match, r_path_coordinate)) {
                            paths_vect[idx].path.push_back(FPoint2{std::stod(match[1].str()), std::stod(match[2].str())});
                            idx++;
                            line = match.suffix();
                        }
                    }
                }
            }
        }
        catch (std::exception &e) {
            LOG(WARNING)
                    << boost::format{"Problem occurs for analysis. test case uuid : %1%.\n "
                                     "Exception: %2%."} % case_ptr->uuid_idx % e.what() ;
        }

        //delete the duplicted target node.
        for(auto &item : paths_vect)
        {
            auto &path = item.path;
            while(path.size()>=2 && *(path.rbegin()) == *(path.rbegin()+1))
                path.pop_back();
        }
    }

    void MAPF::MAPF_DataAnalyzer::ReadCaseResultPIBTV1(
            const MR_TestCase_ptr &case_ptr, const json &j, PathsVector &paths_vect)
    {//regex constants
        std::__cxx11::regex agent_prefix_regex(R"(^\[agent\])", std::__cxx11::regex::icase);
        std::__cxx11::regex path_prefix_regex(R"(^path)", std::__cxx11::regex::icase);
        std::__cxx11::regex size_prefix_regex(R"(^size)", std::__cxx11::regex::icase);

        std::__cxx11::regex agent_id_regex(R"(\d+$)");
        std::__cxx11::regex path_coordinate_regex(R"(\[(-?\d+\.\d+)?\s?,\s?(-?\d+\.\d+)?\])");
        std::__cxx11::regex size_num_regex(R"(\d+$)");
        std::__cxx11::regex double_regex(R"((-?\d+)(\.\d+)?)");

        std::__cxx11::smatch match;

        try {
            boost::filesystem::path log_file_path = boost::filesystem::path(j.at("raw_data_path").get<std::string>());
            if (!boost::filesystem::exists( log_file_path ))
            {
                LOG(WARNING)
                        << boost::format{"Problem occurs for analysis. test case uuid : %1%.\n "
                                  "No raw_data output. \n"
                                  "Copy the error-output-template\n"} % case_ptr->uuid_idx;

//                boost::filesystem::copy_file(TestingFramework::MAPF::MAPF_AnalyzeParam::failed_output_template, log_file_path);
            }


            std::ifstream ss(log_file_path.string());
            std::string line;
            while (getline(ss, line)) {
                if (regex_search(line, agent_prefix_regex)) {
                    TestingFramework::MAPF::PathInfo info;

                    regex_search(line, match, agent_id_regex);   //get id
                    if (match.size() != 1)
                        LOG(WARNING) << boost::format{"@ReadCaseResult, test case uuid: %1%, log file format conflicts."
                                               "no id info"} % case_ptr->uuid_idx;
                    else
                        info.idx = std::stoi(match[0].str());

                    getline(ss, line);   //get path
                    if (regex_search(line, path_prefix_regex)) {
                        while (regex_search(line, match, path_coordinate_regex)) {
                            info.path.push_back(FPoint2{std::stod(match[1].str()), std::stod(match[2].str())});
                            line = match.suffix();
                        }
//                        for(auto tmp : match)
//                            std::cout<<tmp.str()<<std::endl;
//
//                        for (auto iter = match.begin(); iter != match.end(); iter++) {
//                            auto tmp_str = iter->str();
//                            smatch sub_match;
//                            regex_search(tmp_str, sub_match, double_regex);
//                        }

                    } else
                        LOG(WARNING) << boost::format{"@ReadCaseResult, missing path? case uuid: %1%"} % case_ptr->uuid_idx;
                    getline(ss, line);   //goal info , we don't need.
                    getline(ss, line);   //get size
                    if (regex_search(line, size_prefix_regex)) {
                        regex_search(line, match, size_num_regex);
                        //origin size include the right index ,however iter.end() exclude it. So we add 1 to the num;
                        int num = std::stoi(match[0].str()) + 1;
                        auto iter_start = info.path.begin();
                        auto iter_end = info.path.begin() + num;
                        info.path = std::vector<FPoint2>(iter_start, iter_end);
                    } else
                        LOG(WARNING) << boost::format{"@ReadCaseResult, missing size? case uuid: %1%"} % case_ptr->uuid_idx;

                    //add
                    paths_vect.push_back(info);
                }
//                if (regex_search(line, agent_prefix_regex)) {
//                    PathInfo info;
//                    std::cout<<line<<std::endl;
//                }

            }
        }
        catch (std::exception &e) {
            LOG(WARNING)
                    << boost::format{"Problem occurs for analysis. test case uuid : %1%.\n "
                              "Exception: %2%."} % case_ptr->uuid_idx % e.what() ;
        }
    }


}