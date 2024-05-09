//
// Created by hiccup on 2021/2/2.
//

//#include "include/MAPF/MAPF_TestingFile.h"
#include <vector>
#include <string>

#include "boost/filesystem.hpp"
//#include "include/MAPF/MAPF_TestDemo.h"
#include "include/MetamorphicTestingKernel.h"
#include "nlohmann/json.hpp"
#include <iostream>
#include <boost/format.hpp>
#include <include/MAPF/Relations/MAPF_Relation.h>
#include <include/MAPF/Relations/MAPF_RelationBase.h>
#include <include/MAPF/MAPF_TestingFile.h>

#include "CollectionInfos.h"


CollectionInfos::CollectionInfos(
        const std::string &dir, const std::string &MR_filename,
        const std::string &Data_filename) {


    //std::vector<boost::filesystem::path> absolute_path;
    origin_path = boost::filesystem::path(dir);

    if (!boost::filesystem::exists(origin_path)) {
        LOG(ERROR) << boost::format{"Error, can not find the directory : %1%. Return empty vector."} % dir;
        return;
    }

    //fs::directory_iterator end_iter;
    auto end_iter = boost::filesystem::recursive_directory_iterator();
    for (boost::filesystem::recursive_directory_iterator iter(origin_path); iter != end_iter; ++iter) {
        if (boost::filesystem::is_regular_file(iter->status())) {
            boost::filesystem::path _mr_json = iter->path();

            if (_mr_json.extension().string() == ".json" &&
                _mr_json.stem().string().find(MR_filename) != std::string::npos) {
                auto pos = _mr_json.stem().string().find(MR_filename);
                std::string name_tmp = _mr_json.stem().string();
                std::string folder_name = name_tmp.erase(pos, MR_filename.length());


                if (!boost::filesystem::exists(iter->path().parent_path() / folder_name / Data_filename)) {
                    LOG(ERROR) << boost::format{"Error, can not find the file : %1%. Please check the data."} %
                                  (iter->path().parent_path() / folder_name / Data_filename).string();
                } else {
                    this->path_info_vect.push_back(
                            {boost::filesystem::absolute(iter->path()),
                             boost::filesystem::absolute(iter->path().parent_path() / folder_name / Data_filename)});
                }

            }
        }

    }

    //初始化改变名字的字典列表
    InitUpdatedNameDict();
//    return absolute_path;

}

std::string CollectionInfos::Execution(double threshold,
                                       TestingFramework::MAPF::ViolationCriteria::ThresholdTypeEnum threshold_type) {

    using nlohmann::json;
    using boost::format;
    using VC = TestingFramework::MAPF::ViolationCriteria;


    auto output_folder_base = GenerateOutputFolder(std::to_string(threshold), threshold_type);

    //set threshold_type;
    VC::ThresholdType = threshold_type;


    LOG(INFO) << format{"===========================================\n"
                        "Init Execution. Threshold Type: %1%, Value : %2%"} % threshold_type % threshold;
    int idx = 0;
    for (auto &item : path_info_vect) {
        idx++;
        LOG(INFO) << format{"Read percent: %1%/%2%, Threshold Type: %3%, Value : %4%"}
                     % idx
                     % path_info_vect.size()
                     % threshold_type
                     % threshold;
        VLOG(1) << format{
                    "Metamorphic Relation Parameter File: %1%,\n"
                    "Collected Data File: %2% "} %
                   item.MT_param_path.string() % item.collection_data_path.string();


        try {
            json j;
            {
                std::ifstream f(item.MT_param_path.string(), std::ios_base::in);
                f >> j;
                f.close();
            }
            TestingFramework::MT_Parameter _mt = j;

            for (auto &_mr : _mt.MR_list) {
                auto j_hyper = json::parse(_mt.MT_hyper_param);
                TestingFramework::MAPF::MT_HyperPara _hyper = j_hyper;
                auto base_mt_path = output_folder_base /
                                    _hyper.algorithm /
                                    (_mr.name + "-" + _mr.reference_class_name);

                JsonSaveParameter(item, j, _mr, _mt, base_mt_path);

                TestingFramework::MT_Relation_ptr _ptr = TestingFramework::MAPF::MAPF_RelationGenerator(_mr);
                auto _mr_ptr = std::dynamic_pointer_cast<TestingFramework::MAPF::MAPF_MetamorphicRelationBase>(_ptr);

                std::ifstream i(item.collection_data_path.string());
                i >> j;
                i.close();
                for (auto &j_mr_result : j["group"]) {
                    if (j_mr_result["name"].get<std::string>() == _mr.name) {
                        auto &units = j_mr_result["test_units"];
                        for (auto &unit : units) {
                            auto &statistics = unit["statistics"]["statistics"];
//                            bool illegal_case = false;
                            std::vector<double> path_lengths;
                            for (auto &path_info : statistics) {
                                path_lengths.push_back(path_info["max"].get<double>());
//                                if (!path_info["is_legal_execution"].get<bool>()) {
//                                    illegal_case = true;
//                                    break;
//                                }
                            }
//                            if (illegal_case)
//                                unit["statistics"]["pass"] = false;
//                            else
                            unit["statistics"]["pass"] = _mr_ptr->violation_criteria_func(path_lengths, threshold);
                        }
                    }
                }

                JsonSaveDatafiles(item, j, _mt, base_mt_path);

            }
        }
        catch (std::exception &ex) {
            std::cout << ex.what() << std::endl;
            // Extremely low memory situation; don't let exception be unhandled.
        }

    }

    return output_folder_base.string();
}

void CollectionInfos::JsonSaveDatafiles(const CollectionInfos::PathInfo &item,
                                        const nlohmann::json &j,
                                        const TestingFramework::MT_Parameter &mt,
                                        const boost::filesystem::path &base_mt_path) const {
    auto folder_name = item.MT_param_path.filename().stem().string();
    auto pos = folder_name.rfind("MT-para");
    //Since some data_folder are the same, we need to update the new folder.
    auto date_time_str = boost::filesystem::path(mt.tmp_file_path_root).parent_path().filename();
    auto base_data_path = base_mt_path / date_time_str / item.MT_param_path.filename().stem().string().substr(0, pos);
//    base_data_path = boost::filesystem::path( base_data_path.string() + "+" + mr.uuid);
    if (!boost::filesystem::exists(base_data_path)) {
        VLOG(2) << boost::format{"Create Folder: %1%"} % base_data_path;
        boost::filesystem::create_directories(base_data_path);
    }

    std::ofstream o((base_data_path / item.collection_data_path.filename()).string());
    o << j.dump(4);
    o.close();
}

boost::filesystem::path CollectionInfos::GenerateOutputFolder(std::string threshold,
                                                              TestingFramework::MAPF::ViolationCriteria::ThresholdTypeEnum threshold_type) {

    //std::cout<< boost::filesystem::absolute(origin_path).parent_path().relative_path()<<std::endl;
    auto folder_name = origin_path.parent_path().filename().string() + "-ThresholdUpdate";

    std::string threshold_type_name = "";
    if (threshold_type == TestingFramework::MAPF::ViolationCriteria::ThresholdTypeEnum::RatioValue)
        threshold_type_name = "Ratio";
    else
        threshold_type_name = "Absolute";

    std::stringstream ss;
    ss << std::setprecision(3) << threshold << "-" << threshold_type_name;
//    auto _base = ;
    auto new_path = origin_path.parent_path().parent_path() / folder_name / ss.str();

    if (!boost::filesystem::exists(new_path)) {
        VLOG(2) << boost::format{"Create Folder: %1%"} % new_path;
        boost::filesystem::create_directories(new_path);
    }

    return new_path;
}

void CollectionInfos::InitUpdatedNameDict() {

    updated_name_dict = {
            {"IncreaseObstacleRandom",         "IncObstacleRan"},
            {"IncreaseObstacleNoninteraction", "IncObstacleFree"},
            {"AddWindowedPriorityProbe",       "AddPriorityProbe"}
    };

}

std::string CollectionInfos::Execution(std::unordered_map<std::string, double> threshold_dict,
                                       TestingFramework::MAPF::ViolationCriteria::ThresholdTypeEnum threshold_type) {
    using nlohmann::json;
    using boost::format;
    using VC = TestingFramework::MAPF::ViolationCriteria;


    auto output_folder_base = GenerateOutputFolder("ThresholdDict", threshold_type);

    //set threshold_type;
    VC::ThresholdType = threshold_type;

    LOG(INFO) << format{"===========================================\n"
                        "Init Execution. Threshold Type: %1%, Dict Value(map : threshold)"} %
                 threshold_type;
    int idx = 0;
    for (auto &item : path_info_vect) {
        idx++;
        LOG(INFO) << format{"Read percent: %1%/%2%, Threshold Type: %3%, Dict Value (map -> threshold)"}
                     % idx
                     % path_info_vect.size()
                     % threshold_type;
        VLOG(1) << format{
                    "Metamorphic Relation Parameter File: %1%,\n"
                    "Collected Data File: %2% "} %
                   item.MT_param_path.string() % item.collection_data_path.string();

        json j;
        {
            std::ifstream i(item.MT_param_path.string());
            i >> j;
            i.close();
        }
        TestingFramework::MT_Parameter _mt = j;


        for (auto &_mr : _mt.MR_list) {

            auto j_hyper = json::parse(_mt.MT_hyper_param);
            TestingFramework::MAPF::MT_HyperPara _hyper = j_hyper;
            auto base_mt_path = output_folder_base /
                                _hyper.algorithm /
                                (_mr.name + "-" + _mr.reference_class_name);

            JsonSaveParameter(item, j, _mr, _mt, base_mt_path);

            TestingFramework::MT_Relation_ptr _ptr = TestingFramework::MAPF::MAPF_RelationGenerator(_mr);
            auto _mr_ptr = std::dynamic_pointer_cast<TestingFramework::MAPF::MAPF_MetamorphicRelationBase>(_ptr);

            std::ifstream i(item.collection_data_path.string());
            i >> j;
            i.close();
            for (auto &j_mr_result : j["group"]) {
                if (j_mr_result["name"].get<std::string>() == _mr.name) {
                    auto &units = j_mr_result["test_units"];
                    for (auto &unit : units) {
                        auto &map_name = unit["origin_map_name"];
                        //TODO
                        //should I consider the agent_num? although it is easy to change.
                        auto agent_num = int(_hyper.ratio);
                        auto &statistics = unit["statistics"]["statistics"];
                        bool illegal_case = false;
                        std::vector<double> path_lengths;
                        for (auto &path_info : statistics) {
                            path_lengths.push_back(path_info["max"].get<double>());
                            if (!path_info["is_legal_execution"].get<bool>()) {
                                illegal_case = true;
                                break;
                            }
                        }
                        if (illegal_case)
                            unit["statistics"]["pass"] = false;
                        else //TODO
                        {
                            if (threshold_dict.find(map_name) != threshold_dict.end()) {
                                unit["statistics"]["pass"] = _mr_ptr->violation_criteria_func(path_lengths,
                                                                                              threshold_dict[map_name]);
                                VLOG(3) << map_name << threshold_dict[map_name];
                            } else {
                                unit["statistics"]["pass"] = _mr_ptr->violation_criteria_func(path_lengths,
                                                                                              threshold_dict["default"]);
                                VLOG(3) << map_name << threshold_dict[map_name];
                            }

                        }
//                        unit["statistics"]["pass"] = _mr_ptr->violation_criteria_func(path_lengths, threshold);
                    }
                }
            }

            JsonSaveDatafiles(item, j, _mt, base_mt_path);

        }
    }

    return output_folder_base.string();
}

void
CollectionInfos::JsonSaveParameter(const CollectionInfos::PathInfo &item, nlohmann::json &j,
                                   const TestingFramework::MR_Parameter &_mr,
                                   const TestingFramework::MT_Parameter &mt,
                                   const boost::filesystem::path &base_mt_path) {
    if (!boost::filesystem::exists(base_mt_path)) {
        VLOG(2) << boost::format{"Create Folder: %1%"} % base_mt_path;
        boost::filesystem::create_directories(base_mt_path);
    }

    std::ifstream i(item.MT_param_path.string());
    i >> j;
    i.close();

    //check name should be updated?
    for (auto &item : updated_name_dict) {
        if (_mr.reference_class_name.find(item.first) != std::string::npos) {
            for (nlohmann::json &j_mr_item : j["MR_list"])
                if (j_mr_item["name"].get<std::string>() == _mr.name) {
                    j_mr_item["reference_class_name"] = item.second;
                    break;
                }
            break;
        }
    }
    auto date_time_str = boost::filesystem::path(mt.tmp_file_path_root).parent_path().filename();
    if (!boost::filesystem::exists(base_mt_path / date_time_str)) {
        VLOG(2) << boost::format{"Create Folder: %1%"} % (base_mt_path / date_time_str);
        boost::filesystem::create_directories(base_mt_path / date_time_str);
    }

    auto save_path_str = (base_mt_path / date_time_str / item.MT_param_path.filename()).string();
    VLOG(2) << boost::format{"Save MT-param to: %1%"} % save_path_str;
    std::ofstream o(save_path_str);
    o << j.dump(4);
    o.close();
}
