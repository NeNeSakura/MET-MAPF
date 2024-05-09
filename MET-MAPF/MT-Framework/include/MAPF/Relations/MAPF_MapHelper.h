//
// Created by hiccup on 2020/7/27.
//

#pragma once

#include <string>
#include <vector>
#include <sstream>
#include "../MAPF_Point2D.hpp"


namespace TestingFramework {
    namespace MAPF {
        using std::vector;
        using std::string;

        class MapHelper
        {
        protected:
            vector <string> map_data;
            size_t width;
            size_t height;
            vector<char> obstacles = {'@','T'}; //obstacles
        public:
            void MapReader(const std::string& map_data_str);
            bool CheckPointSafe(FPoint2 pos);

        };



    }
}