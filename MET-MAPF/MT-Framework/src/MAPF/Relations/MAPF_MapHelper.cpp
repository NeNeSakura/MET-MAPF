//
// Created by hiccup on 2020/7/27.
//

#include <regex>
#include "../../../include/MAPF/Relations/MAPF_MapHelper.h"
#include "easyloggingpp/easylogging++.h"


namespace TestingFramework {
    namespace MAPF {
        using boost::format;

        bool MapHelper::CheckPointSafe(FPoint2 pos) {
            auto x = size_t(ceil(pos.x));
            auto y = size_t(ceil(pos.y));
            if (x >= width || y >= height || x < 0 || y < 0) {
                LOG(ERROR) << format{"illegal input pos: [%1%,%2%],Map:[%3%,%4%]"} % pos.x % pos.y % width % height;
                return false;
            } else
                return !std::any_of(obstacles.begin(), obstacles.end(),
                                    [&](const char i) -> bool { return map_data[y][x] == i; });

        }

        void MapHelper::MapReader(const std::string &map_data_str) {

            map_data.clear();
            bool mapStart = false;
            std::regex r_map = std::regex(R"(^map)");
            std::smatch results;

            std::istringstream istr(map_data_str);
            string line;

            while (std::getline(istr, line)) {
                if (mapStart)
                    map_data.push_back(line);
                if (std::regex_search(line, results, r_map)) mapStart = true;
            }

            if (map_data.size() == 0) {
                height = 0;
                width = 0;
            } else {
                height = map_data.size();
                width = map_data.begin()->size();
            }
        }
    }

}


