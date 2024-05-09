//
// Created by hiccup on 2020/5/7.
//

#include "../../../include/MAPF/Relations/MAPF_RelationBase.h"
#include <limits>
#include <boost/filesystem.hpp>

#include "boost/format.hpp"
#include "../../../include/MAPF/Operator/MAPF_Operator.h"
#include "../../../include/MAPF/Relations/MAPF_RelationHelper.h"
#include "../../../include/MAPF/DataHandler/MAPF_DataAnalyzer.h"


namespace TestingFramework {
    namespace MAPF {

        string MAPF_MetamorphicRelationBase::TestCaseParamGenerator(string json_raw_str) {

            nlohmann::json j = nlohmann::json::parse(json_raw_str);

//            string map_file_name = j["case_path_root"].get<string>() +
//                                   j["origin_case_name"].get<string>();
            int seed = j["seed"].get<int>();
            unsigned agent_num = j["agent_num"].get<unsigned>();
            string map_raw = j["map"].get<string>();
            string task_raw = j["task"];
            nlohmann::json j_o = j["param"];
            string tmp_address = j_o["log_path"].get<string>();
            string map_file_name = j_o["map_file_name"];
            string task_file_name = j_o["task_file_name"];
            string para_file_name = j_o["para_file_name"];


            string algorithm = j["algorithm"].get<string>();
            int overstep_limit = j["overstep_limit"].get<int>();
            int overtime_limit = j["overtime_limit"].get<int>();


            namespace fs = boost::filesystem;
            fs::path map_path = fs::path(map_file_name);
            fs::path task_path = fs::path(task_file_name);

            // generate task file
            {
                std::ofstream os;
                os.open(task_file_name, std::ios::out | std::ios::trunc);
                if (!os.is_open())
                    LOG(ERROR) << format{"error~ can not find or create the file : %1%"} % task_file_name;
                os << task_raw << std::endl;
                os.close();
            }


            // generate map file
            {
                std::ofstream os;
                os.open(map_file_name, std::ios::out | std::ios::trunc);
                if (!os.is_open())
                    LOG(ERROR) << format{"error~ can not find or create the file : %1%"} % map_file_name;
                os << map_raw;
                os.close();
            }

#ifdef _SUT_VERSION_
            if(_SUT_VERSION_ == "2.0")
            // generate PIBT parameter file;
            {
                string task_string= "";
                auto task_raw_j = json::parse(task_raw);
                for (auto& agent : task_raw_j) {
                    task_string += "\n" + std::to_string(agent["start"][0].get<int>()) + "," + std::to_string(agent["start"][1].get<int>()) + "," +
                                   std::to_string(agent["goal"][0].get<int>()) + "," + std::to_string(agent["goal"][1].get<int>());
                }

                // %1%:map address
                // %2%:agent num
                // %3%:seed
                // %4%:max_step
                // %5%:max_time
                // %6%:task_str

                auto foramt_str =
                        boost::format{
//                                ## ==== necessary ====
//                                ## .map file, put your maps in /map folder
                                "map_file=%1%\n"
//
//                                ## number of agents
                                "agents=%2%\n"
//
//                                ## ==== custom =======
//                                ## seed of randomization
                                "seed=%3%\n"
//
//                                ## true  -> random starts/goals
//                                ## false -> use custom starts/goals
                                "random_problem=0\n"
//
//                                ## timestep limit
                                "max_timestep=%4%\n"
//
//                                ## computation time limit, ms
                                "max_comp_time=%5%\n"
//                                ## custom starts/goals
//                                ## (x_s, y_s, x_g, y_g)
//                                ## (0, 0): left-top corner
//                                # 0,0,4,0
//                                # 3,1,3,0
                                "%6%"
                                }
                        % ("./" + map_path.filename().string())
                        % agent_num
                        % seed
                        % overstep_limit
                        % overtime_limit
                        % task_string;

                std::ofstream os;
                os.open(para_file_name, std::ios::out | std::ios::trunc);
                if (!os.is_open())
                    LOG(ERROR) << format{"error~ can not find or create the file : %1%"} % para_file_name;
                os << foramt_str << std::endl;
                os.close();
            }
#else
            // generate PIBT parameter file;
            {
                // %1%:map address
                // %2%:agent num
                // %3%:scenariofile(task list)
                // %4%:seed
                // %5%:algorithm selection
                // %6%:overtime_limits
                auto foramt_str =
                        boost::format{
                                "===params of problem setting===\n"
                                "// choose problem\n"
                                "// option: { MAPF, MAPF_STATION, MAPD, IMAPF, IMAPF_STATION, I_MAPF_FAIR }\n"
                                "PROBLEM_TYPE=MAPF\n"
                                "\n"
                                "// choose solver\n"
                                "// option: { PIBT, HCA, WHCA, PPS, CBS, ECBS, iECBS, TP, winPIBT }\n"
                                "SOLVER_TYPE=%5%\n"
                                "\n"
                                "// choose map file\n"
                                "field=%1%\n"
                                "\n"
                                "// number of agents\n"
                                "agentnum=%2%\n"
                                "\n"
                                "// timestep limitation\n"
                                "timesteplimit=%6%\n"
                                "\n"
                                "// number of tasks, for MAPD, IMAPF\n"
                                "tasknum=500\n"
                                "\n"
                                "// task frequency for MAPD\n"
                                "taskfrequency=1\n"
                                "\n"
                                "// use scenario, choose {0, 1}\n"
                                "scenario=1\n"
                                "\n"
                                "// scenario file\n"
                                "scenariofile=%3%\n"
                                "\n"
                                "// seed for randomness\n"
                                "seed=%4%\n"
                                "\n"
                                "// save log or not, choose {0, 1}\n"
                                "log=1\n"
                                "\n"
                                "// print log, choose {0, 1}\n"
                                "printlog=0\n"
                                "\n"
                                "// save task information(start goal)\n"
                                "savetask=0\n"
                                "\n"
                                "// print implementation time or not, choose {0, 1}\n"
                                "printtime=1\n"
                                "\n"
                                "\n"
                                "===params of solvers===\n"
                                "// calculate paths beforehand by Warshall-Floyd, choose {0, 1}\n"
                                "WarshallFlpoyd=0\n"
                                "\n"
                                "// Indepent Detection for CBS and ECBS, choose {0, 1}\n"
                                "ID=0\n"
                                "\n"
                                "// window size of WHCA\n"
                                "window=5\n"
                                "\n"
                                "// suboptimal factor of ECBS\n"
                                "suboptimal=1.2\n"
                                "\n"
                                "// for winPIBT, iterative use\n"
                                "softmode=1\n"
                                "\n"
                                "===params of visualizatoin===\n"
                                "// show icon initially, choose {0, 1}\n"
                                "showicon=0\n"
                                "\n"
                                "// icon file\n"
                                "icon=./material/peterpan.png"}
                        % ("./" + map_path.filename().string())
                        % agent_num
                        % ("./" + task_path.filename().string())
                        % seed
                        % algorithm
                        % overtime_limits;

                std::ofstream os;
                os.open(para_file_name, std::ios::out | std::ios::trunc);
                if (!os.is_open())
                    LOG(ERROR) << format{"error~ can not find or create the file : %1%"} % para_file_name;
                os << foramt_str << std::endl;
                os.close();
            }
#endif
            return j_o.dump();
        }

        //output to MR_TestCase::input_args
        vector<string>
        MAPF_MetamorphicRelationBase::originCaseParamGenerator(MR_Parameter &mr_param, MT_Parameter &mt_param,
                                                               string &origin_case_name) {
            VLOG(2) <<
                    format{"Generate Origin Case Parameters data with "
                           "relation name : %1%, type : %2%, origin case name: %3%"} % mr_param.name %
                    mr_param.reference_class_name % origin_case_name;

            json j_hyper_param = json::parse(mt_param.MT_hyper_param);

            //see MAPF_TestDemo @MT_Para_Demo() MT hyper param;
            TestingFramework::MAPF::MAPF_RelationHelper helper(j_hyper_param["shrink_mission_pos"].get<int>());
            string str = mt_param.case_path_root + origin_case_name;
            //nlohmann::json j_hyper_param = nlohmann::json::parse(mt_param.MT_hyper_param);

            auto agent_ratio = j_hyper_param["ratio"].get<double>();
            helper.InitTaskData(str);
            auto raw_map = helper.RawMapReader(str);

            vector<string> out;
            for (auto &seed : mr_param.testunit_seeds) {

                auto raw_task = helper.getRandomStartGoal_RawJson(agent_ratio,
                                                                  RandomHelper::GetCoreGenerator(seed));
                auto agent_num = json::parse(raw_task).size();

                auto uuid = boost::uuids::to_string(boost::uuids::random_generator()());

                string tmp_address = mt_param.tmp_file_path_root;
                string map_file_name = tmp_address + (format{"tmp_map-%1%.map"} % uuid).str();
                string task_file_name = tmp_address + (format{"tmp_task-%1%.json"} % uuid).str();
                string para_file_name = tmp_address + (format{"tsample-param-%1%.txt"} % uuid).str();

                nlohmann::json para_jo;
                para_jo["para_file_name"] = para_file_name;
                para_jo["task_file_name"] = task_file_name;
                para_jo["map_file_name"] = map_file_name;
                para_jo["log_path"] = tmp_address;

                nlohmann::json j2;

                if ((seed > (unsigned) std::numeric_limits<int>::max()))
                    j2["seed"] = (RandomHelper::GetRandomInt());
                else
                    j2["seed"] = seed;
//                j2["seed"] = (seed > (unsigned)std::numeric_limits<int>::max())? (RandomHelper::GetSeed()):seed;
                j2["origin_case_name"] = origin_case_name;
                j2["case_path_root"] = mt_param.case_path_root;
                j2["tmp_file_path_root"] = mt_param.tmp_file_path_root;
                j2["agent_num"] = agent_num;
                j2["map"] = raw_map;
                j2["task"] = raw_task;
                j2["algorithm"] = j_hyper_param["algorithm"].get<string>();
                j2["param"] = para_jo;
                j2["mr_param"] = mr_param;
                j2["uuid"] = uuid;
                j2["origin_map_name"] = origin_case_name;
                j2["overstep_limit"] = j_hyper_param["overstep_limit"];
                j2["overtime_limit"] = j_hyper_param["overtime_limit"];

                out.push_back(j2.dump());
            }

            return out;
        }

        //子类实现相关参数的更新
        vector<string> MAPF_MetamorphicRelationBase::followCaseParamGenerator(string &origin_case_param,
                                                                              const string &origin_case_result) {
            auto origin_vect = initFollowCaseParamVector(origin_case_param);

            for (auto iter = origin_vect.begin(); iter != origin_vect.end(); iter++) {
                //json j = json::parse(*iter);
                //这里的RelationOeration 是本地操作，json会被修改
                operator_ptr->RelationOperation(*iter, origin_case_result);
                //*iter = j.dump();
            }

            return origin_vect;
        }

        //该部分实现json字符串基本信息更新
        vector<string>
        MAPF_MetamorphicRelationBase::initFollowCaseParamVector(string &origin_case_param) const {//get followcase num;
            json j = json::parse(origin_case_param);
            json &para_jo = j["param"];
            json j_param = json::parse(operator_ptr->operator_param);

            int dup_num = 1;
            try {
                dup_num = j_param.at("num").get<int>();
            }
            catch (...) {
                //VLOG(2)<< "No parameter 'num', set the num = 1";
            }

            auto param = j["mr_param"].get<MR_Parameter>();
            VLOG(2) <<
                    format{"Generate Follow Up Cases Parameter data with "
                           "relation name : %1%, type : %2%, origin case name: %3%, Random seed: %4%"} % param.name %
                    param.reference_class_name % j["origin_case_name"].get<string>() % j["seed"].get<int>();

            //init output vector
            std::vector<string> out_vect;

            for (int i = 0; i < dup_num; i++) {
                //update seed
                int seed = RandomHelper::GetRandomInt();
                j["seed"] = seed;
                //update uuid
                auto uuid = boost::uuids::to_string(boost::uuids::random_generator()());
                j["uuid"] = uuid;
                //update save path
                string tmp_address = para_jo["log_path"].get<string>();
                para_jo["para_file_name"] = tmp_address + (format{"tsample-param-%1%.txt"} % uuid).str();
                para_jo["task_file_name"] = tmp_address + (format{"tmp_task-%1%.json"} % uuid).str();
                para_jo["map_file_name"] = tmp_address + (format{"tmp_map-%1%.map"} % uuid).str();
                out_vect.push_back(j.dump());
            }

            return out_vect;
        }


//        void MAPF_MetamorphicRelationBase::initParameters(MR_Parameter &mr_param) {
//            this->MR_param = mr_param;
//        }


    }
}