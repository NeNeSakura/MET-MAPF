#include <iostream>
#include "nlohmann/json.hpp"

#include <string>
#include "include/MetamorphicTestingKernel.h"
#include "include/MAPF/MAPF_TestDemo.h"
#include "boost/filesystem.hpp"
#include "boost/asio.hpp"
//#include "boost/asio/thread_pool.hpp"
//#include "boost/asio/post.hpp"


#include <indicators/progress_bar.hpp>
#include <indicators/progress_spinner.hpp>
#include <indicators/cursor_control.hpp>
#include <indicators/block_progress_bar.hpp>

#include <vector>
#include "include/MAPF/MAPF_TestingFile.h"
#include "CollectionInfos.h"


using nlohmann::json;
using namespace TestingFramework;
using namespace TestingFramework::MAPF;
namespace fs = boost::filesystem;

std::vector<MR_Parameter> getVector(size_t test_unit_num);

#define ELPP_THREAD_SAFE
#define ELPP_STL_LOGGING
//#define ELPP_FORCE_USE_STD_THREAD
//#ifndef ELPP_DEFAULT_LOGGER
//#   define ELPP_DEFAULT_LOGGER "update_manager"
//#endif
#include "easyloggingpp/easylogging++.h"


void rm_all_files(const fs::path & dir_path,        // in this directory,
                const std::string & file_name)      // search for this name,
{
    std::vector<fs::path> results;
    if ( !exists( dir_path ) )return;
    fs::recursive_directory_iterator end_itr; // default construction yields past-the-end
    for ( fs::recursive_directory_iterator itr( dir_path );
          itr != end_itr;
          ++itr )
    {
        if ( is_directory(itr->status()) )
        {
            continue;
        }
        else if (  itr->path().leaf() == file_name ) // see below
        {
            results.push_back(itr->path());
        }
    }

    for(const auto& p : results)
        fs::remove(p);
    return;
}


int main(int argc, char *argv[]) {
//    START_EASYLOGGINGPP(argc, argv);
    //el::Configurations conf(R("/home/hiccup/Append/ZXY-Tmp//my_log.conf"));


    using nlohmann::json;
    using boost::posix_time::ptime;
    using boost::posix_time::microsec_clock;
    using boost::filesystem::path;

    std::cout << "Threshold Rewrite Start!" << std::endl;

    std::string now_str = to_iso_string(boost::posix_time::second_clock::local_time());

    std::vector<MR_Parameter> mr_para_vect;
    auto MT_root_path = boost::filesystem::current_path().parent_path().parent_path().parent_path();
    std::string MT_root_str = MT_root_path.c_str();

//    auto _base = path("/home/hiccup/Append/ZXY-Tmp/MT-Debug/20210201T151114/");
//    auto _base = path("/home/hiccup/Appendix/ZXY-Tmp/MetamorphicTestingData/20210206T222338-Result/");
//    auto _base = path("/home/hiccup/Appendix/PIBT-Output-V2/Output/MetamorphicTestingData");
    auto _base = path("/home/hiccup/Appendix/PIBT-Output-V2/9.14.2022/MetamorphicTestingData-origin/");


    if(argc == 3 && string(argv[1]) == "-i"){
        _base = path(argv[2]);
    }


    if (!(fs::is_directory(_base)) && (fs::exists(_base))) {
        LOG(ERROR) << format{"error~ can not find the directory : %1%"} % _base;
        exit(-1);
    }
    CollectionInfos info(_base.string(), "MT-para", "data_map.json");

// debug
//    for (auto threshold : {0.05, 0.1, 0.15, 0.2, 0.25, 0.3, .35, .4})
//    {
//        info.Execution(std::to_string(threshold), TestingFramework::MAPF::ViolationCriteria::RatioValue);
//    }
    vector<double> threshold_vect ;
    for(auto i = 0 ; i< 15; i++)
    {
        threshold_vect.push_back(0.02*i + 0.02);
    }
//    threshold_vect.push_back(0.1);

    for (auto threshold : threshold_vect)
    {
        const auto output_str = info.Execution(threshold, TestingFramework::MAPF::ViolationCriteria::RatioValue);

        auto file = "python3 " + MT_root_str + "/MetamorphicTestingFramework/DataAnalysisScript/ReadFile.py";

        std::string str = " -i " + output_str;
        std::string instruction = file + str;
        std::cout << "PY instruction:"<< instruction << std::endl;
        std::thread t1([=]() -> void {
//            system("conda activate MAPF");
            system(instruction.c_str());
        });
        t1.join();

        rm_all_files(output_str,"data_map.json");
    }
//    boost::asio::thread_pool pool(9);
//    for (double threshold : threshold_vect) {
//        boost::asio::post(pool,
//                          [&]() {
//                              info.Execution(threshold, TestingFramework::MAPF::ViolationCriteria::RatioValue);
//                          });
//        usleep(100000);
//    }
//    pool.join();
//
//    for (auto threshold : {5.0, 9.948684123514713, 10.0, 15.0, 20.0, 25.0}) {
//        boost::asio::post(pool,
//                          [&]() {
//                              info.Execution(threshold, TestingFramework::MAPF::ViolationCriteria::AbsoluteValue);
//                          });
//        sleep(1);
//    }
//    pool.join();


//    std::unordered_map<string,double> abc = {{"Berlin_1_256.map",5.5},{"Boston_0_256.map",6.0},{"Paris_1_256.map",7.0},{"default",5.0}};
//    info.Execution( abc, TestingFramework::MAPF::ViolationCriteria::AbsoluteValue);
//    info.Execution( std::to_string(5.0) , TestingFramework::MAPF::ViolationCriteria::AbsoluteValue);

//    std::vector<std::thread> thread_vect;
//    thread_vect.clear();
//    for (auto threshold : {5.0, 9.948684123514713, 10.0, 15.0, 20.0, 25.0}) {
//        thread_vect.push_back(std::thread([&]() {
//            info.Execution(threshold, TestingFramework::MAPF::ViolationCriteria::AbsoluteValue);
//        }));
//        sleep(1);
//    }
//    for (auto &thread : thread_vect) {
//        thread.join();
//    }
//
//    thread_vect.clear();
//    for (auto threshold : {0.05, 0.093, 0.1, 0.15, 0.2, 0.25, 0.3, .35}) {
//        thread_vect.push_back(std::thread([&]() {
//            info.Execution(threshold, TestingFramework::MAPF::ViolationCriteria::RatioValue);
//        }));
//        sleep(1);
//    }
//    for (auto &thread : thread_vect) {
//        thread.join();
//    }

    return 0;

}

