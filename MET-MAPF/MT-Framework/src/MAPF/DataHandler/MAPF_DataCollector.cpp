//

// Created by hiccup on 2020/5/14.
//

#include "../../../include/MAPF/DataHandler/MAPF_DataCollector.h"

#include "boost/date_time/posix_time/posix_time.hpp" //include all types plus i/o
#include "boost/filesystem.hpp"
#include "../../../include/MAPF/DataHandler/MAPF_AnalyzeParam.h"
#include <boost/iterator/filter_iterator.hpp>
#include <regex>
#include <boost/filesystem.hpp>


namespace TestingFramework {

    //TODO check refpath bugs
    void MAPF::MAPF_DataCollector::DataCollectionPreprocessor(std::string const &param_raw_str,
                                                              const MR_TestCase_ptr &case_ptr) {
        using boost::posix_time::ptime;
        using boost::posix_time::from_iso_string;
        using boost::posix_time::to_time_t;
        namespace fs = boost::filesystem;


        json j = json::parse(param_raw_str);

        //uuid
        string uuid = j["uuid"].get<string>();
        //paths
        fs::path base_path(j["folder"].get<string>());
        fs::path origin_path = base_path;
        origin_path /= "log";
//        auto new_raw_data_base_path = fs::path(target_folder_path);
//        new_raw_data_base_path /= "results";
//        auto new_raw_param_base_path = fs::path(target_folder_path);
//        new_raw_param_base_path /= "params";
//        auto tmp_log_4_run = new_raw_param_base_path;
//        tmp_log_4_run /= "log";
//
//        if (!fs::exists(new_raw_data_base_path))
//            fs::create_directories(new_raw_data_base_path);
//        if (!fs::exists(new_raw_param_base_path))
//            fs::create_directories(new_raw_param_base_path);
//        if (!fs::exists(tmp_log_4_run))
//            fs::create_directories(tmp_log_4_run);

        json o_arg_input = json::parse(case_ptr->input_args);
        auto map_name = o_arg_input["origin_map_name"].get<std::string>();
        auto new_raw_base_path = fs::path(target_folder_path);
        new_raw_base_path /= map_name;
        auto new_raw_data_base_path = new_raw_base_path;
        new_raw_data_base_path /="result";
        auto new_raw_param_base_path = new_raw_base_path;
        new_raw_param_base_path /="params";
        auto tmp_log_4_run = new_raw_param_base_path;
        tmp_log_4_run /= "log";

        if (!fs::exists(new_raw_data_base_path))
            fs::create_directories(new_raw_data_base_path);
        if (!fs::exists(new_raw_param_base_path))
            fs::create_directories(new_raw_param_base_path);
        if (!fs::exists(tmp_log_4_run))
            fs::create_directories(tmp_log_4_run);


        if (!(fs::exists(origin_path) && fs::is_directory(origin_path))) {
            LOG(WARNING) << format{"Path : '%1%' is not exist"} % origin_path;
            fs::create_directories(origin_path);
            sleep(1);
        }


        string start_time_iso_str = j["case_start_time"].get<string>();
        string end_time_iso_str = j["case_end_time"].get<string>();
        ptime start_time_p = from_iso_string(start_time_iso_str);
        ptime end_time_p = from_iso_string(end_time_iso_str);
        time_t start_time = to_time_t(start_time_p);
        time_t end_time = to_time_t(end_time_p);




        //output files filter;
        auto pred_data_files = [](const fs::directory_entry &p) {
            if (!fs::is_regular_file(p))
                return false;
            else
                return p.path().extension() == ".txt";
        };

        auto pred_param_files = [](const fs::directory_entry &p) {

            if (!fs::is_regular_file(p))
                return false;
            else {
                auto str = p.path().extension();
                if (str == ".json" || str == ".map" || str == ".txt")
                    return true;
            }
            return false;
        };


        //move log
        std::vector<fs::path> data_files;
//        auto base_path = fs::path(target_folder_path);
//        auto log_path = base_path.append("log");
        fs::directory_iterator dir_data(origin_path), dir_param(base_path), dir_end;

        std::copy(boost::iterators::make_filter_iterator(pred_data_files, dir_data, dir_end),
                  boost::iterators::make_filter_iterator(pred_data_files, dir_end, dir_end),
                  std::back_inserter(data_files)
        );


        json o;
        auto a = json::array();
        for (auto &x : data_files) {
            time_t t_write = boost::filesystem::last_write_time(x);
            if (t_write >= start_time && t_write <= end_time) {
                //if(x.stem().string().find(case_ptr->uuid_idx) != string::npos){
                auto new_path = new_raw_data_base_path;
//                new_path /= x.filename();
                new_path /= "result-"+ case_ptr->uuid_idx;
                // if renaming a file
                // (ultimately through the rename() library call, whether it's wrapped up in boost:: or anything else)
                // fails because the source and destination are on different file systems,
                // the only option is to then copy the file and delete the original
                // after verifying that the copy was complete and successful.
                // This is what /bin/mv does - it first tries a rename(),
                // and if the error code returned by it's failure indicates a cross-device link situation,
                // it falls back to a copy and remove scenario.
                try {
                    fs::rename(x, new_path);
                }
                catch (boost::filesystem::filesystem_error) {
                    fs::copy(x, new_path);
                    fs::remove(x);
                }
                //原则上有且仅有一个文件会挪动
                o["raw_data_path"] = new_path.string();

            }

//            if (x.filename().string().find(uuid) != string::npos ) {
//                auto new_path = new_raw_data_base_path;
//                new_path /= x.filename();
//                fs::rename(x, new_path);
//                //原则上有且仅有一个文件会挪动
//                o["raw_data_path"] = new_path.string();
//
//            }

        }

        data_files.clear();
        fs::path txtpath;
        std::regex appendix_regex(".txt$");
        std::smatch match;

        std::copy(boost::iterators::make_filter_iterator(pred_param_files, dir_param, dir_end),
                  boost::iterators::make_filter_iterator(pred_param_files, dir_end, dir_end),
                  std::back_inserter(data_files)
        );
        for (auto &x : data_files) {
//            time_t t_write = boost::filesystem::last_write_time(x);
//            if (t_write >= start_time && t_write <= end_time) {
//                auto new_path = new_raw_param_base_path;
//                new_path /= x.filename();
//                fs::rename(x, new_path);
//                //原则上有且仅有3个文件 * n 会挪动
//                a.push_back(new_path.string());
//            }
            if (x.filename().string().find(uuid) != string::npos) {
                auto new_path = new_raw_param_base_path;
                new_path /= x.filename();
                // if renaming a file
                // (ultimately through the rename() library call, whether it's wrapped up in boost:: or anything else)
                // fails because the source and destination are on different file systems,
                // the only option is to then copy the file and delete the original
                // after verifying that the copy was complete and successful.
                // This is what /bin/mv does - it first tries a rename(),
                // and if the error code returned by it's failure indicates a cross-device link situation,
                // it falls back to a copy and remove scenario.
                try {
                    fs::rename(x, new_path);
                }
                catch (boost::filesystem::filesystem_error) {
                    fs::copy(x, new_path);
                    fs::remove(x);
                }
                if (std::regex_search(x.filename().string(), match, appendix_regex))
                    txtpath = new_path;
                //原则上有且仅有3个文件 * n 会挪动
                a.push_back(new_path.string());
            }
        }
        string algorithm = o_arg_input["algorithm"].get<string>();
        o["input_path"] = a;
        o["microseconds_period"] = (end_time_p - start_time_p).total_microseconds();
        o["uuid"] = case_ptr->uuid_idx;
#ifdef _SUT_VERSION_
        o["execution"] = (format{"cd %3% && %2% -i ./%1% -o ./log/%4% -s %6% && %5% ./log/%4%" } %
                          txtpath.filename().string() %
                          MAPF_AnalyzeParam::execution_path_CLI %
                          txtpath.branch_path().string()%
                          "result.txt"%
                          MAPF_AnalyzeParam::execution_path_GUI%
                          algorithm).str();

#else
        o["execution"] = (format{"cd %1% && %2% -p %3%"}
                          % txtpath.branch_path().string()
                          % MAPF::MAPF_AnalyzeParam::execution_path_GUI
                          % txtpath.filename().string()).str();
#endif
        case_ptr->output_args = o.dump(2);

        //move

    }

    MAPF::MAPF_DataCollector::MAPF_DataCollector(const std::string &path) : IDataCollector(path) {
        namespace fs = boost::filesystem;

        using boost::posix_time::ptime;
        using boost::posix_time::time_duration;
        using boost::posix_time::seconds;

        time_out_threshould = seconds(MAPF::MAPF_AnalyzeParam::time_out_seconds_threshould).total_microseconds();

        try {
            fs::path origin_path = fs::current_path();
            if (path == "") {
                origin_path /= "CollectedData";
            } else {
                origin_path = fs::path(path);
            }
            origin_path /= boost::posix_time::to_iso_extended_string(boost::posix_time::second_clock::local_time());
            target_folder_path = origin_path.string();
            if (!fs::exists(target_folder_path))
                fs::create_directories(target_folder_path);
        }
        catch (fs::filesystem_error e) {
            LOG(ERROR) << format{"@MAPF_DataCollector throw exceptions : %1%"} % e.what();
            VLOG(1)
                << format{"ErrorInfo : %1% \n Path1 : %2% \n Path2 : %3% \n"} % e.what() % e.path1() % e.path2();
        }
    }

    void MAPF::MAPF_DataCollector::DataCollection(TestingFramework::MR_ConfigTree &root) {
        namespace fs = boost::filesystem;

        LOG(INFO) << "Start data collection process.";
        ZipDatas(root);

        ConstructDataMapJsonFile(root);
        ConstrcutFailedCaseJsonFile(root);

        LOG(INFO) << "Data collection construction process complete.";

    }

    void MAPF::MAPF_DataCollector::ZipDatas(MR_ConfigTree &tree) {
        LOG(INFO) << "Prepare to zip datas";
//        system("pwd");

        namespace fs = boost::filesystem;
        auto origin_pwd = fs::current_path();
        auto base_path = fs::path(target_folder_path);
        fs:current_path(base_path);
        auto zip_file = base_path/"origin_datas.zip";
        std::vector<fs::path> data_folders;
        for(auto& entry : boost::make_iterator_range(fs::directory_iterator(target_folder_path), {}))
        {
            std::cout << entry << "\n";
            data_folders.push_back(entry.path());
        }

        std::string instruction = "zip -rm " + zip_file.string();
        for(auto& path : data_folders)
        {
            instruction += " " + fs::relative(path,base_path).string();
        }
//        std::cout<< (instruction.c_str());
        system(instruction.c_str());
        fs::current_path(origin_pwd);
        LOG(INFO) << "Zip work complete";


    }


    void MAPF::MAPF_DataCollector::ConstructDataMapJsonFile(const MR_ConfigTree &root) const {
        auto base_path = boost::filesystem::path(target_folder_path);
        base_path /= "data_map.json";

        json j;
        j["path"] = base_path.string();
        j["group"] = json::array();
        for (auto &group : root.group_list) {
            json o_group = {{"name",       group->relation_param.name},
                            {"uuid",       group->relation_param.uuid},
                            {"type",       group->relation_param.reference_class_name},
                            {"param",      json::parse(group->relation_param.MR_para_str)},
                            {"test_units", json::array()}};
            for (auto &unit : group->unit_list) {
                json o_arg = json::parse(unit->origin_case->input_args);

                json o_unit = {{"uuid",   unit->uuid_idx},
                               {"seed",   unit->seed},
                               {"origin_map_name",    o_arg["origin_map_name"]},
                               {"origin", json::parse(unit->origin_case->output_args)},
                               {"follow", json::array()}};
                for (auto &follow_case : unit->follow_cases_list) {
                    o_unit["follow"].push_back(json::parse(follow_case->output_args));
                }
                o_unit["statistics"] = json::parse(unit->relation_satisfied_record);

                o_group["test_units"].push_back(o_unit);
            }
            j["group"].push_back(o_group);
        }

        std::ofstream os;
        os.open(base_path.string(), std::ios::out | std::ios::trunc);
        if (!os.is_open())
            LOG(ERROR) << format{"error~ can not find or create the file : %1%."} % j;
        os << (j.dump(2));
        os.close();
    }

    void MAPF::MAPF_DataCollector::ConstrcutFailedCaseJsonFile(const MR_ConfigTree &root) {
        namespace fs = boost::filesystem;
        auto base_path = boost::filesystem::path(target_folder_path);
        base_path /= "failed_data_map.json";

//        auto raw_data_base_path = fs::path(target_folder_path);
//        raw_data_base_path /= "results";
//        auto raw_param_base_path = fs::path(target_folder_path);
//        raw_param_base_path /= "params";

//        auto failed_data_base_path = fs::path(target_folder_path);
//        failed_data_base_path /= "failed";


        json j = json::array();
        for (auto &group : root.group_list) {
            for (auto &unit : group->unit_list) {
                {
                    json j_arg_origin = json::parse(unit->origin_case->output_args);
                    j_arg_origin["unit_uuid"] = unit->uuid_idx;
                    j_arg_origin["relation_name"] = unit->relation_ptr->instance_name;
                    //see DataCollectionPreprocessor to get the definition.
                    long period = j_arg_origin["microseconds_period"].get<long>();
                    if (period > time_out_threshould)
                        j.push_back(j_arg_origin);
                }

                for (auto &follow_case : unit->follow_cases_list) {
                    auto j_arg_follow = json::parse(follow_case->output_args);
                    j_arg_follow["unit_uuid"] = unit->uuid_idx;
                    j_arg_follow["relation_name"] = unit->relation_ptr->instance_name;
                    //see DataCollectionPreprocessor to get the definition.
                    long period = j_arg_follow["microseconds_period"].get<long>();
                    if (period > time_out_threshould)
                        j.push_back(j_arg_follow);
                }
            }
        }

        std::ofstream os;
        os.open(base_path.string(), std::ios::out | std::ios::trunc);
        if (!os.is_open())
            LOG(ERROR) << format{"error~ can not find or create the file : %1%."} % j;
        os << (j.dump(2));
        os.close();
    }



}


