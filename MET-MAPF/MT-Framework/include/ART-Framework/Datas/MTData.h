//
// Created by hiccup on 2021/5/31.
//

#pragma once

#include "../../MAPF/Relations/MAPF_RelationHelper.h"


namespace ARTTestingFramework {

//    using MTItemData_ptr = std::shared_ptr <MTItemData>
    using std::string;
    using TestingFramework::MAPF::MAPF_Scenario;
    using TestingFramework::json;


    //TODO? need cache?!
    class MTItemData {
    public:
        long idx;
        MAPF_Scenario scenario;

    public:
        MTItemData()
        {
            idx = -1;
        }

        MTItemData(const string &MR_param_json_str,long idx)
        {
            scenario = getScenario(MR_param_json_str);
//            idx = boost::uuids::to_string(boost::uuids::random_generator()());
            this->idx = idx;

        }

        MAPF_Scenario getScenario(const string &MR_param_json_str) {
            //            ScenarioJsonReader()
            return TestingFramework::MAPF::MAPF_RelationHelper::ScenarioJsonReader(MR_param_json_str);
        }

    };


    class MeasureItemData{
    public:
        MeasureItemData()
        {
            measure_value = 0;
        }

        MeasureItemData(TestingFramework::MR_TestUnit_ptr origin_data_ptr,long idx)
        :origin_data_ptr(origin_data_ptr),MT_itemData(origin_data_ptr->origin_case->input_args,idx)
        {
            measure_value = 0;
            json o_arg_2 = json::parse(origin_data_ptr->origin_case->input_args);
            map_name = o_arg_2["origin_map_name"].get<std::string>();
        }

        MeasureItemData(TestingFramework::MR_TestUnit_ptr origin_data_ptr,long idx, const std::string &map_str)
                :origin_data_ptr(origin_data_ptr),MT_itemData(origin_data_ptr->origin_case->input_args,idx)
        {
            measure_value = 0;
            map_name = map_str;
        }



    public:
        MTItemData MT_itemData;
        TestingFramework::MR_TestUnit_ptr origin_data_ptr;
        string map_name;
        double measure_value;
//        bool is_item_success;

    };

//    class MeasureData{
//    public:
//        std::vector<MeasureItemData> measure_data;
//        std::vector<TestingFramework::MR_TestUnit_ptr> input_data; //input data
//        std::vector<TestingFramework::MR_TestUnit_ptr> output_data; //output data
//
//
//        MeasureData(const TestingFramework::MR_ConfigTree &origin_data)
//        {
//            if(origin_data.group_list.size() !=1)
//                throw std::range_error("origin_data.group_list.size() should be 1!");
//            for(auto &g : origin_data.group_list)
//            {
//                input_data = g->unit_list;
//            }
//        }
//    };

}