//
// Created by hiccup on 2020/5/18.
//

#pragma once

#include <vector>
#include <string>
#include "../../API/IDataAnalyzer.hpp"
#include "../../Param/TestingConfigTree.h"
#include "../MAPF_Point2D.hpp"


namespace TestingFramework {

    using nlohmann::json;
    namespace MAPF {

        struct MR_CaseStatics {
            string case_uuid;
            int max_length;
            double medium_length;
            double mean_length;
            bool is_legal_execution;
        };


        struct PathInfo {
            int idx;
            std::vector<FPoint2> path;
        };

        using PathsVector = vector<MAPF::PathInfo>;

        struct RelationAnalyzeInfo {
            PathsVector origin;
            std::vector<PathsVector> follows;
            double threshold;

            RelationAnalyzeInfo(const string &origin_result, const vector <string> &follow_results);

            static int GetLength(const PathsVector &path_vect);
            static double GetMedium(const PathsVector &path_vect);
            static double GetMean(const PathsVector &path_vect);
            //static bool CheckCollision(const PathsVector &path_vect, const MAPF_Map &map);

            static vector <MR_CaseStatics>
            getUnitStatics(const MR_TestUnit_ptr &unit_ptr, const vector <string> &follow_up_results);

        };

        void to_json(json &j, const PathInfo &p);

        void from_json(const json &j, PathInfo &p);

        //============================================================//

        //key:unit_uuid;
        //value: unitStatics;
        //first case is the origin, others are follows.
        using MR_UnitStatics_MAP = std::map<string, vector < MR_CaseStatics>>();

        void to_json(json &j, const MR_CaseStatics &p);

        void from_json(const json &j, MR_CaseStatics &p);

    }




}

