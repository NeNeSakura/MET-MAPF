//
// Created by hiccup on 2021/2/2.
//

#pragma once

#include <unordered_map>
#include <string>


class CollectionInfos {

public:
    struct PathInfo {
        boost::filesystem::path MT_param_path;
        boost::filesystem::path collection_data_path;
    };
    std::vector<PathInfo> path_info_vect;
    boost::filesystem::path origin_path;
    std::unordered_map<std::string,std::string> updated_name_dict;

public:
    CollectionInfos(
            const std::string &dir,
            const std::string &MR_filename,
            const std::string &Data_filename);


    std::string Execution(double threshold, TestingFramework::MAPF::ViolationCriteria::ThresholdTypeEnum threshold_type);


    std::string Execution(std::unordered_map<std::string, double> threshold_dict, TestingFramework::MAPF::ViolationCriteria::ThresholdTypeEnum threshold_type);
protected:

    void InitUpdatedNameDict();

//    boost::filesystem::path
//    GenerateOutputFolder(double threshold, TestingFramework::MAPF::ViolationCriteria::ThresholdTypeEnum threshold_type);

    boost::filesystem::path
    GenerateOutputFolder(std::string threshold, TestingFramework::MAPF::ViolationCriteria::ThresholdTypeEnum threshold_type);

//    void JsonSaveDatafiles(const PathInfo &item, const nlohmann::json &j, const boost::filesystem::path &base_mt_path) const;

    void JsonSaveParameter(const PathInfo &item, nlohmann::json &j, const TestingFramework::MR_Parameter &_mr,
                           const TestingFramework::MT_Parameter &mt,
                           const boost::filesystem::path &base_mt_path);

    void JsonSaveDatafiles(const PathInfo &item, const nlohmann::json &j, const TestingFramework::MT_Parameter &mt,
                           const boost::filesystem::path &base_mt_path) const;
};



