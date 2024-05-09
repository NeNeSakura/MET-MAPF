#include <iostream>
#include "nlohmann/json.hpp"
#include <string>
#include <include/RandomHelper.h>
#include "include/MetamorphicTestingKernel.h"
#include "include/MAPF/MAPF_TestDemo.h"

#include <indicators/progress_spinner.hpp>
#include <indicators/cursor_control.hpp>

#include <vector>
#include <include/MAPF/MAPF_TestingFile.h>

//using nlohmann::json ;


int main(int argc, char *argv[]) {

    auto MT_root_path = boost::filesystem::current_path().parent_path().parent_path().parent_path();
    std::string MT_root_str = MT_root_path.c_str();

    std::string MT_map_based_str = MT_root_str + "/mapf-map/";

    std::string MT_output_base_str = MT_root_str + "/Output";
//    std::string MT_output_base_str = "/home/hiccup/Appendix/ZXY-Tmp";

    std::string MT_output_str = MT_output_base_str + "/RandomTestingData";
//    std::string MT_output_str = MT_output_base_str + "/ARTTestingData/ART-Random";
//    std::string MT_output_str = MT_output_base_str + "/RandomTestingData/10-group-test";


    std::string MT_tested_base_str = MT_root_str + "/pibt2";
//    std::string MT_tested_base_str = MT_root_str + "/pibt";


    std::cout << "MT_root_str:" << MT_root_str << std::endl;
    std::cout << "Mt_output_str:" << MT_output_str << std::endl;


    START_EASYLOGGINGPP(argc, argv);
    el::Configurations conf("./my_log.conf");

    using nlohmann::json;
    std::cout << "This is RandomTesting" << std::endl;
    using boost::posix_time::ptime;
    using boost::posix_time::microsec_clock;

    std::string now_str = to_iso_string(boost::posix_time::microsec_clock::local_time());
//    std::vector<std::string> algorithms = {"PIBT","PIBT_PLUS","HCA","PushAndSwap"};
//    std::vector<std::string> algorithms = {"PIBT"};
    std::vector<std::string> algorithms = {"PIBT",  "PIBT_PLUS", "HCA"};

    std::vector<double> agent_num = {10,20,30,40,50,60,70,80,90,100};
//    std::vector<double> agent_num = {10, 20, 30, 40, 50};
//    std::vector<double> agent_num = {20, 40, 60, 80, 100};
//    std::vector<double> agent_num = {1};

    std::vector<std::string> maps = {
            "Berlin_1_256.map",
            "Boston_0_256.map",
            "Paris_1_256.map",
            "Shanghai_0_256.map",
            "Milan_0_256.map",
            "brc202d.map",
            "den312d.map",
            "den520d.map",
            "ht_chantry.map",
            "ht_mansion_n.map",
            "lak303d.map",
            "ost003d.map",
            "warehouse-10-20-10-2-1.map",
            "warehouse-10-20-10-2-2.map",
            "maze-32-32-4.map",
            "maze-128-128-10.map",
            "random-32-32-10.map",
            "random-64-64-10.map",
            "room-64-64-8.map",
            "room-64-64-16.map",

//            "maze-32-32-2.map",
//            "random-32-32-20.map",
//            "random-64-64-20.map",
//            "room-32-32-4.map",
    };


//  #step 1 go get random seeds for filtering, no repeat execution(repeat = 0)
    TestingFramework::RandomHelper::InitSeed();
    std::vector<unsigned> random_list;

//    size_t repeat_unit = 10;
//    size_t repeat_unit = 100;
    size_t repeat_unit = 380;
    for (size_t i = 0; i < repeat_unit; i++)
        random_list.push_back(TestingFramework::RandomHelper::GetRandomInt());


    size_t idx = 0;
    for (auto &name : algorithms) {
        for (auto num: agent_num) {
            idx++;
            TestingFramework::MAPF::MAPF_TestingFile tester =
                    {
                            {
                                    false,
                                    num,
                                    name,
                                    2000, //overstep_limits step count
                                    3000 // millionseconds.
                            },
                            random_list.size(),
                            0,      //origin + followup = 20;
                            MT_output_str + "/" + now_str + "/" + name + "/",
                            MT_map_based_str,
                            MT_output_base_str + "/Tmp/" + now_str + "/",
                            "",
                            idx,
                            algorithms.size() * agent_num.size(),
                            maps,
                            MT_tested_base_str + "/visualizer/bin/visualizer",
                            MT_tested_base_str + "/build/mapf",
                            "my_log.conf",
                            false,
                            true,
                            0.15,
                            TestingFramework::MAPF::ViolationCriteria::RatioValue,
//                            MT_root_str + "/MetamorphicTestingFramework/MT-Framework/error-output-template.txt"
                    };
            tester.Init();

            json j = tester.RandomTestingFileGenerator(random_list);

            auto my_stream = TestingFramework::MetamorphicTestingKernel(
                    tester.tmp_file_path_root + "MT-para.json");
            my_stream.MT_param_str = j.dump(4);
            my_stream.initTesting(TestingFramework::MAPF::MAPF_RelationGenerator);

            my_stream.execution();

            my_stream.collectResultInfo();

            my_stream.analyzeResultInfo();
        }
    }


    {
        auto file = "python3 " + MT_root_str + "/MetamorphicTestingFramework/DataAnalysisScript/ReadFile.py";

        std::string str = " -i " + MT_output_str + "/" + now_str;
        std::string instruction = file + str;
        std::cout << "PY instruction:" << instruction << std::endl;

        std::thread t1([=]() -> void {
            system(instruction.c_str());
        });
        t1.join();
    }



    return 0;

}



