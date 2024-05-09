//
// Created by hiccup on 2020/5/5.
//

#include "../../../include/MAPF/Operator/MAPF_Operator.h"

void TestingFramework::MAPF::scenario_viewer(const TestingFramework::MAPF::MAPF_Scenario &scenario) {
    const MAPF_Map &map = scenario.map_info;
    auto &data = map.map_data;
    const MAPF_Task &task = scenario.task_info;

    auto tmp = data;
    for(auto &item : task)
    {
        //auto item = task[0];
        if (tmp[item.start.y][item.start.x] == 'T')
            tmp[item.start.y][item.start.x] = 'X';
        else
            tmp[item.start.y][item.start.x] = 's';
        if (tmp[item.goal.y][item.goal.x] == 'T')
            tmp[item.goal.y][item.goal.x] = 'X';
        else
            tmp[item.goal.y][item.goal.x] = 'g';
    }

    for (auto &line : tmp)
        std::cout << line << std::endl;
}
