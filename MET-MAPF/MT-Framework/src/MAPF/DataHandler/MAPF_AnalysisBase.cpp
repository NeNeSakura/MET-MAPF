//
// Created by hiccup on 2020/5/18.
//

#include <regex>
#include <boost/filesystem.hpp>
#include <string>
#include <numeric>
#include <limits>
#include "../../../include/MAPF/Operator/MAPF_Operator.h"
#include "../../../include/MAPF/DataHandler/MAPF_DataAnalyzer.h"
#include "../../../include/MAPF/DataHandler/MAPF_AnalysisBase.h"
#include "../../../include/MAPF/DataHandler/MAPF_AnalyzeParam.h"


namespace TestingFramework {


    MAPF::RelationAnalyzeInfo::RelationAnalyzeInfo(const string &origin_result, const vector<string> &follow_results) {
        {
            json j = json::parse(origin_result);
            for (auto &jo : j)
                origin.push_back(jo);
        }
        follows.clear();
        for (auto &str : follow_results) {
            json j = json::parse(str);
            PathsVector tmp_vect;
            for (auto &jo : j)
                tmp_vect.push_back(jo);
            follows.push_back(tmp_vect);
        }

        threshold = MAPF::MAPF_AnalyzeParam::validation_threshold;
    }


    int MAPF::RelationAnalyzeInfo::GetLength(const PathsVector &path_vect) {
        if (path_vect.size() == 0)
            return -1;
        else if (path_vect.size() == 1)
            return path_vect[0].path.size();
        else

            return std::max_element(path_vect.begin(),
                                    path_vect.end(),
                                    [](const TestingFramework::MAPF::PathInfo &lhs, const TestingFramework::MAPF::PathInfo &rhs) {
                                        return lhs.path.size() < rhs.path.size();
                                    })->path.size();
    }

    double MAPF::RelationAnalyzeInfo::GetMedium(const PathsVector &path_vect) {

        if (path_vect.size() == 0)
            return -1;
        else if (path_vect.size() == 1)
            return path_vect[0].path.size();

        vector<size_t> length_vect;
        length_vect.clear();
        std::transform(path_vect.begin(), path_vect.end(), std::back_inserter(length_vect),
                       [&](const TestingFramework::MAPF::PathInfo &info) { return info.path.size(); });

        if (length_vect.size() != 0) {
            std::sort(length_vect.begin(), length_vect.end());
            if (length_vect.size() % 2 == 1)
                return length_vect[(length_vect.size() + 1) / 2];
            else
                return (length_vect[(length_vect.size() / 2)] + length_vect[(length_vect.size() / 2) - 1]) / 2.0;
        } else {
            LOG(ERROR) << "@RelationAnalyzeInfo::GetMedium , path_vect is 0";
            return std::numeric_limits<double>::infinity();
        }

    }

    double MAPF::RelationAnalyzeInfo::GetMean(const PathsVector &path_vect) {
        if (path_vect.size() == 0)
            return -1;
        else if (path_vect.size() == 1)
            return path_vect[0].path.size();

        vector<size_t> length_vect;
        length_vect.clear();
        std::transform(path_vect.begin(), path_vect.end(), std::back_inserter(length_vect),
                       [&](const TestingFramework::MAPF::PathInfo &info) { return info.path.size(); });

        if (length_vect.size() != 0)
            return std::accumulate(length_vect.begin(),
                                   length_vect.end(), 0.0) / length_vect.size();
        else {
            LOG(ERROR) << "@RelationAnalyzeInfo::GetMean , path_vect is 0";
            return std::numeric_limits<double>::infinity();
        }
    }


    vector<MAPF::MR_CaseStatics>
    MAPF::RelationAnalyzeInfo::getUnitStatics(const MR_TestUnit_ptr &unit_ptr,
                                              const vector<string> &follow_up_results) {
        TestingFramework::MAPF::RelationAnalyzeInfo info(unit_ptr->origin_result, follow_up_results);
        std::vector<TestingFramework::MAPF::MR_CaseStatics> case_statics_list;

        {
            try {
                TestingFramework::MAPF::MR_CaseStatics caseStatics{
                        unit_ptr->origin_case->uuid_idx,
                        RelationAnalyzeInfo::GetLength(info.origin),
                        RelationAnalyzeInfo::GetMean(info.origin),
                        RelationAnalyzeInfo::GetMedium(info.origin),
                        unit_ptr->origin_case->is_legal_execution
                };
                case_statics_list.push_back(caseStatics);
            }
            catch (exception &e) {
                LOG(WARNING) << format{"loss follow output, uuid : %1%, exception: %2%"} %
                                unit_ptr->origin_case->uuid_idx % e.what();

                TestingFramework::MAPF::MR_CaseStatics caseStatics{
                        unit_ptr->origin_case->uuid_idx,
                        -1,
                        -1,
                        -1,
                        false
                };
                case_statics_list.push_back(caseStatics);
            }

        }

        for (auto i = 0; i < info.follows.size(); i++) {
            try {
                auto &follow = info.follows[i];
                TestingFramework::MAPF::MR_CaseStatics caseStatics{
                        unit_ptr->follow_cases_list[i]->uuid_idx,
                        RelationAnalyzeInfo::GetLength(follow),
                        RelationAnalyzeInfo::GetMean(follow),
                        RelationAnalyzeInfo::GetMedium(follow),
                        unit_ptr->follow_cases_list[i]->is_legal_execution,
                };
                case_statics_list.push_back(caseStatics);
            }
            catch (exception &e) {
                LOG(WARNING) << format{"loss follow output, uuid : %1%, exception: %2%"} %
                                unit_ptr->follow_cases_list[i]->uuid_idx % e.what();

                TestingFramework::MAPF::MR_CaseStatics caseStatics{
                        unit_ptr->follow_cases_list[i]->uuid_idx,
                        -1,
                        -1,
                        -1,
                        false,
                };
                case_statics_list.push_back(caseStatics);
            }
        }
        return case_statics_list;
    }

//    bool MAPF::RelationAnalyzeInfo::CheckCollision(const PathsVector &path_vect, const MAPF_Map &map) {
//
//        if(path_vect.size() == 0)
//            return false;
//        else if (path_vect .size() == 1)
//            return false;
//    }


    void MAPF::to_json(json &j, const MAPF::PathInfo &p) {

        j["idx"] = p.idx;
        j["path"] = p.path;
    }

    void MAPF::from_json(const json &j, MAPF::PathInfo &p) {

        p.path.clear();
        auto j_path = j["path"];
        for (auto &item : j_path) {
            auto pos = item.get<FPoint2>();
            p.path.push_back(pos);
        }
        p.idx = j["idx"].get<int>();
    }

    void MAPF::to_json(json &j, const MAPF::MR_CaseStatics &p) {
        j["uuid"] = p.case_uuid;
        j["max"] = p.max_length;
        j["medium"] = p.medium_length;
        j["mean"] = p.mean_length;
        j["is_legal_execution"] = p.is_legal_execution;
    }

    void MAPF::from_json(const json &j, MAPF::MR_CaseStatics &p) {
        j["uuid"].get_to(p.case_uuid);
        j["max"].get_to(p.max_length);
        j["medium"].get_to(p.medium_length);
        j["mean"].get_to(p.mean_length);
        j["is_legal_execution"].get_to(p.is_legal_execution);
    }
}