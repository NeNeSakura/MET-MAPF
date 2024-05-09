//
// Created by hiccup on 2020/5/9.
//

#include "../../../include/MAPF/Execution/MAPF_Execution.h"
#include <thread>
#include "boost/format.hpp"
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <boost/process.hpp>
#include <boost/asio.hpp>
#include "../../../include/TimeHelper.h"
#include <signal.h>

namespace TestingFramework {
    namespace MAPF {


        void MAPF_Execution::testingCodeExecutionFunc(MR_TestCase_ptr case_ptr) {

            // check file is exists

            // call target testing program with parameter

            // result file moving(no data collection)

            VLOG(3) << "Code Execution";
            json j = json::parse(case_ptr->input_args);

            bool exist = FileExist(j["param"]["para_file_name"].get<string>()) &&
                         FileExist(j["param"]["task_file_name"].get<string>()) &&
                         FileExist(j["param"]["map_file_name"].get<string>());
            if (!exist) {
                LOG(ERROR) << "file incomplete! Jumped";
                return;
            }

            //check task_file_availabilty
            //check map_file_availability;
            if (!isScenarioLegal(j))
                return;

            namespace fs = boost::filesystem;
            fs::path p(j["param"]["para_file_name"].get<string>());
            format format_str;

#ifdef _SUT_VERSION_
            string algorithm = j["algorithm"].get<string>();
            if(_SUT_VERSION_ == "2.0")
            {
                string opt_file_name = setCurrentTime() + ".txt";
                if (MAPF_AnalyzeParam::exec_type == MAPF_AnalyzeParam::CLI) {
                    format_str = format{"cd %3% && %2% -i %1% -o ./log/%4% -s %5%" } %
                                 j["param"]["para_file_name"].get<string>() %
                                 MAPF_AnalyzeParam::execution_path_CLI %
                                 p.parent_path().string()%
                                 opt_file_name%
                                 algorithm;
                } else if (MAPF_AnalyzeParam::exec_type == MAPF_AnalyzeParam::GUI) {
                    format_str = format{"cd %3% && %2% -i %1% -o ./log/%4% -s %6% && %5% ./log/%4%" } %
                                 j["param"]["para_file_name"].get<string>() %
                                 MAPF_AnalyzeParam::execution_path_CLI %
                                 p.parent_path().string()%
                                 opt_file_name%
                                 MAPF_AnalyzeParam::execution_path_GUI%
                                 algorithm;
                }
            }
#else
            if (MAPF_AnalyzeParam::exec_type == MAPF_AnalyzeParam::CLI) {
                format_str = format{"cd %3% && %2% -p %1%"} %
                             j["param"]["para_file_name"].get<string>() %
                             MAPF_AnalyzeParam::execution_path_CLI %
                             p.parent_path().string();
            } else if (MAPF_AnalyzeParam::exec_type == MAPF_AnalyzeParam::GUI) {
                format_str = format{"cd %3% && %2% -p %1%"} %
                             j["param"]["para_file_name"].get<string>() %
                             MAPF_AnalyzeParam::execution_path_GUI %
                             p.parent_path().string();
            }
#endif

            VLOG(3) << format_str;
            namespace bp = ::boost::process;

            std::error_code ec;
            //boost process case
//            bp::ipstream pipe_stream;
//            bp::child c("gcc --version", bp::std_out > pipe_stream);
//            std::string line;
//            while (pipe_stream && std::getline(pipe_stream, line) && !line.empty())
//                std::cerr << line << std::endl;
//            c.wait();


            if(MAPF_AnalyzeParam::exec_type == MAPF_AnalyzeParam::CLI) {
                bp::ipstream pipe_stream;
                bp::child c(bp::search_path("sh"), "-c", format_str.str().c_str(), bp::std_out > pipe_stream);
//            bp::child c(bp::search_path("sh"), "-c", "xclock", bp::std_out > pipe_stream);
//            bp::child c2("pgrep -P "+ std::to_string(pid), bp::std_out > pipe_stream);
                LOG(INFO) << "Worker thread is executed." << "Worker PID:" << c.native_handle();
                if(!c.wait_for(std::chrono::milliseconds {j["overtime_limit"].get<int>()},ec))
                {
                    LOG(WARNING) << "Worker thread seems fall into dead loops. Calling termination." << "Worker PID:" << c.native_handle();
                    pid_t pid = c.native_handle();
//                    int result = kill(pid,SIGKILL);
                    system( ("pkill -P " + std::to_string(pid)).c_str());
                    c.terminate();
                    LOG(WARNING) << "Worker thread has been killed. " << " Worker PID: " << pid;
                }
                else {
                    std::string line;
                    while (pipe_stream && std::getline(pipe_stream, line) && !line.empty())
                        std::cout << line << std::endl;
                    LOG(INFO) << "Worker thread works well. " << "Worker PID:" << c.native_handle();
                }
//                c.wait();
            }
            else {  //GUI
                  system(format_str.str().c_str()); //maybe get crushed!
            }

//            boost::posix_time::time_duration timeout = boost::posix_time::milliseconds(10000);
//            auto timeout = boost::chrono::seconds(1);
//            boost::thread t1([&]() -> void {
//                try{
//                  system(format_str.str().c_str());
//
//                }
//                catch(boost::thread_interrupted const& )
//                {
//                    LOG(WARNING) << "Interruption Called and prepare to terminate the thread";
//
//                    std::terminate();
//                }
//            });
//            if(!t1.try_join_for(timeout)) {
//                //Not finished;
//                LOG(WARNING) << "Worker thread seems fall into dead loops. Calling interruption.";
//                t1.interrupt();
//                VLOG(3) << "Execution ID: " << t1.get_id() << " is going to be interrupted!";
//            }
//            //for interruption.
//            t1.try_join_for(boost::chrono::microseconds(1));

//            t1.join();
        }

        bool MAPF_Execution::isScenarioLegal(const json &j) const {
            MAPF_Scenario scenario = MAPF_RelationHelper::ScenarioReader(j["map"].get<string>(),
                                                                         j["task"].get<string>());
            bool flag = true;
            for (auto &item : scenario.task_info) {
                int s_x = item.start.x;
                int s_y = item.start.y;
                int g_x = item.goal.x;
                int g_y = item.goal.y;
                char s = scenario.map_info.map_data[s_y][s_x];
                char g = scenario.map_info.map_data[g_y][g_x];
                if (s == 'T' || s == '@') {
                    flag = false;
                    LOG(WARNING) << format{"Scenario task conflicts with map. [%1%]"} % item.start;
                    VLOG(1) << item.start;
                    break;
                }
                if (g == 'T' || g == '@') {
                    flag = false;
                    LOG(WARNING) << format{"Scenario task conflicts with map. [%1%]"} % item.goal;
                    VLOG(1) << item.goal;
                    break;
                }
            }
            if (!flag)
                LOG(WARNING) << "Illegal scenario, skip......";
            return flag;
        }

        bool MAPF_Execution::FileExist(const string &filename) const {
            std::ifstream f(filename.c_str());
            return f.good();
        }

//        string MAPF_Execution::originTestCaseResultReaderFunc(MR_TestCase_ptr case_ptr) {
//            // check file is exists
//            // result file moving(no data collection)
//
//            VLOG(3) << "ReaderFunc Execution";
//
//            return std::string();
//
//        }

        void MAPF_Execution::executeOriginCase(MR_TestUnit_ptr unit_ptr) {
            MAPF_ExecutionBase::executeOriginCase(unit_ptr);
        }

        void MAPF_Execution::executeFollowCases(MR_TestUnit_ptr unit_ptr) {
            MAPF_ExecutionBase::executeFollowCases(unit_ptr);
        }

        void MAPF_Execution::lazyGenerateFollowUpTestCase(MR_TestUnit_ptr unit_ptr) {
            MAPF_ExecutionBase::lazyGenerateFollowUpTestCase(unit_ptr);
        }

        string MAPF_Execution::originTestCaseResultReaderFunc(MR_TestCase_ptr case_ptr) {
            return MAPF_ExecutionBase::originTestCaseResultReaderFunc(case_ptr);
        }

        string MAPF_Execution::setCurrentTime() {
            char buff[27] = "";
            time_t now = time(NULL);
            timeval curTime;
            gettimeofday(&curTime, NULL);
            struct tm* pnow = localtime(&now);
            sprintf(buff, "%04d-%02d-%02d-%02d-%02d-%02d-%06d",
                    pnow->tm_year + 1900, pnow->tm_mon + 1,
                    pnow->tm_mday, pnow->tm_hour, pnow->tm_min, pnow->tm_sec,
                    curTime.tv_usec);
            return std::string(buff);
        }
    }
}

