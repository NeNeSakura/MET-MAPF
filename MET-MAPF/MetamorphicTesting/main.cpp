#include <iostream>
#include "nlohmann/json.hpp"

#include <string>
#include <vector>

#include "include/MetamorphicTestingKernel.h"
#include "include/MAPF/MAPF_TestDemo.h"
#include "MR_ParameterGenerator.h"
#include <indicators/progress_spinner.hpp>
#include <indicators/cursor_control.hpp>
#include "include/MAPF/MAPF_TestingFile.h"
#include "MR_ParameterGeneratorV2.h"


using namespace TestingFramework;
using namespace TestingFramework::MAPF;

std::vector<MR_Parameter> getVector(size_t test_unit_num);

int main(int argc, char *argv[]) {
    START_EASYLOGGINGPP(argc, argv);

    auto MT_root_path = boost::filesystem::current_path().parent_path().parent_path().parent_path();
    std::string MT_root_str = MT_root_path.c_str();

    std::string MT_map_based_str = MT_root_str + "/mapf-map/";

    std::string MT_output_base_str = MT_root_str + "/Output";
//    std::string MT_output_base_str = "/home/hiccup/Appendix/ZXY-Tmp";

    std::string MT_output_str = MT_output_base_str + "/MetamorphicTestingData/";

    std::string MT_tested_base_str = MT_root_str + "/pibt2";
//    std::string MT_tested_base_str = MT_root_str + "/pibt";
//    std::string MT_tested_base_str = MT_root_str + "/pibt-v1.3/pibt";


    std::cout << "MT_root_str:" << MT_root_str << std::endl;
    std::cout << "Mt_output_str:" << MT_output_str << std::endl;
    std::cout << "This is Metamorphic Testing." << std::endl;

    using nlohmann::json;
    using boost::posix_time::ptime;
    using boost::posix_time::microsec_clock;

    std::string now_str = to_iso_string(boost::posix_time::microsec_clock::local_time());
    //    std::vector<std::string> algorithms = {"PIBT","PIBT_PLUS","HCA","PushAndSwap"};
    std::vector<std::string> algorithms = {"PIBT",  "PIBT_PLUS", "HCA"};
//    std::vector<std::string> algorithms = {"PIBT_PLUS"};

//    std::vector<double> agent_num = {10, 20, 30, 40, 50};
//    std::vector<double> agent_num = {20, 40, 60, 80, 100};
//    std::vector<double> agent_num = {100};
    std::vector<double> agent_num = {10,20,30,40,50,60,70,80,90,100};


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

    TestingFramework::RandomHelper::InitSeed();
    // don't forget you will independently execute the expriment 10 times
    // MR_ParameterGeneratorV2 MR_param_gene(100);
    MR_ParameterGeneratorV2 MR_param_gene(10);
    // MR_ParameterGeneratorV2 MR_param_gene(5);

//    std::vector<MR_Parameter> mr_para_vect = MR_param_gene.getRandomVector();
    std::vector<MR_Parameter> mr_para_vect = MR_param_gene.getVector();


    size_t idx = 0;
    for (auto &name : algorithms) {
        for (auto mr: mr_para_vect) {
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
                                mr.testunit_seeds.size(),
                                0,      //repeat_case : useless para for metamorphic testing part.
                                MT_output_str + "/" + now_str + "/" + name + "/"
                                + mr.name + "-" + mr.reference_class_name + "/",
                                MT_map_based_str,
                                MT_output_base_str + "/Tmp/" + now_str + "/",
                                MT_tested_base_str,
                                idx,
                                algorithms.size() * agent_num.size() * mr_para_vect.size(),
                                maps,
                                MT_tested_base_str + "/visualizer/bin/visualizer",
                                MT_tested_base_str + "/build/mapf",
                                "my_log.conf",
                                false,
                                true,
//                                9.948684123514713,
//                                ViolationCriteria::AbsoluteValue
                                0.1,
                                ViolationCriteria::RatioValue
                        };
                tester.Init();


                std::vector<MR_Parameter> mr_vect = {mr};
                json j = tester.MetamorphicTestingFileGenerator(mr_vect);

                auto my_stream = TestingFramework::MetamorphicTestingKernel(
                        tester.tmp_file_path_root + "MT-para.json");
                my_stream.MT_param_str = j.dump(4);
                my_stream.initTesting(TestingFramework::MAPF::MAPF_RelationGenerator);

                my_stream.execution();

                my_stream.collectResultInfo();

                my_stream.analyzeResultInfo();
            }
        }
    }

    auto file = "python3 " + MT_root_str + "/MetamorphicTestingFramework/DataAnalysisScript/ReadFile.py";

    std::string str = " -i " + MT_output_str + "/" + now_str;
    std::string instruction = file + str;
    std::cout << "PY instruction:" << instruction << std::endl;

    std::thread t1([=]() -> void {
        system(instruction.c_str());
    });
    t1.join();

    return 0;

}

