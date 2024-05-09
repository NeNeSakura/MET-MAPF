//
// Created by hiccup on 2020/5/6.
//

#include "../../include/MAPF/MAPF_TestDemo.h"
#include <regex>
#include <string>
#include <boost/format.hpp>
#include <nlohmann/json.hpp>
#include <boost/date_time/posix_time/time_formatters.hpp>
#include <boost/filesystem.hpp>
#include "easyloggingpp/easylogging++.h"
#include "../../include/RandomHelper.h"
#include "../../include/MAPF/Operator/MAPF_CoordinateParam.hpp"
#include "../../include/MAPF/Operator/MAPF_MissionParam.hpp"
#include "../../include/MAPF/Operator/MAPF_InterativeParam.hpp"
#include "../../include/MAPF/Operator/MAPF_ObstacleParam.hpp"

namespace TestingFramework {
    namespace MAPF {

        using boost::format;
        using std::string;

        string MAPF_TestDemo::RawMapReader(std::string &filename) {
            // read file
            std::ifstream ss(filename);
            if (!ss) {
                LOG(ERROR) << "error@SimpleGrid::setBasicParams, file "
                           << filename
                           << " does not exist"
                           << "\n";
                std::exit(1);
            }

            string line;
            string str;
            while (getline(ss, line)) {
                str += line + "\n";
            }
            return str;
        }

        void MAPF_TestDemo::MapReader(std::string &filename) {

            // read file
            std::ifstream ss(filename);
            if (!ss) {
                LOG(ERROR) << "error@SimpleGrid::setBasicParams, file "
                           << filename
                           << " does not exist"
                           << "\n";
                std::exit(1);
            }

            vector<FPoint2> data;
            std::string line;
            std::smatch results;

            int w = -1;
            int h = -1;
            int j = 0; // height
            char s;
            string type = "";
            bool mapStart = false;

            std::regex r_type = std::regex(R"(type)");
            std::regex r_height = std::regex(R"(height\s(\d+))");
            std::regex r_width = std::regex(R"(width\s(\d+))");
            std::regex r_map = std::regex(R"(map)");

            // read fundamental graph params

            using boost::is_any_of;
            using boost::split;
            using boost::token_compress_on;
            while (getline(ss, line)) {
                if (std::regex_match(line, results, r_height)) {
                    h = std::stoi(results[1].str());
                }
                if (std::regex_match(line, results, r_width)) {
                    w = std::stoi(results[1].str());
                }
                if (type == "" && std::regex_match(line, results, r_type)) {
                    vector<string> split_str_vect;
                    split(split_str_vect, line, is_any_of(" "), token_compress_on);
                    type = split_str_vect[1];
                }
                if (mapStart) {
                    // width check
                    if (line.size() != w) {
                        LOG(ERROR) << format{"@ScenarioReader: width is invalid, should be %1%"} % w;
                        std::exit(1);
                    }
                    for (int i = 0; i < w; ++i) {
                        s = line[i];
                        //                        id = j * w + i;
                        if (s == 'T' or s == '@')
                            continue;
                        data.push_back(FPoint2(i, j));
                    }
                    ++j;
                }

                if (std::regex_match(line, results, r_map))
                    mapStart = true;
            }

            // height check
            if (j != h) {
                LOG(ERROR) << format{"@ScenarioReader, height is invalid, should be %1% but actually %2%"} % h, j;
                std::exit(1);
            }

            starts = vector<FPoint2>(data.begin(), data.end());
            goals = vector<FPoint2>(data.begin(), data.end());
        }

        vector<std::pair<FPoint2, FPoint2>> MAPF_TestDemo::getRandomStartGoal(unsigned num, std::mt19937 &MT) {
            if (num > starts.size() || num > goals.size()) {
                std::cout << "error@Graph::getStartGoal, over node size"
                          << "\n";
                std::exit(1);
            }

            vector<std::pair<FPoint2, FPoint2>> data;
            vector<FPoint2> ss(starts.size());
            vector<FPoint2> gs(goals.size());
            bool flg;

            std::copy(starts.begin(), starts.end(), ss.begin());
            std::copy(goals.begin(), goals.end(), gs.begin());

            while (true) {
                data.clear();
                std::shuffle(ss.begin(), ss.end(), MT);
                std::shuffle(gs.begin(), gs.end(), MT);

                flg = true;
                for (int i = 0; i < num; ++i) {
                    if (ss[i] != gs[i]) {
                        data.push_back({ss[i], gs[i]});
                    } else {
                        flg = false;
                        break;
                    }
                }

                if (flg)
                    break;
            }

            return data;
        }

        string MAPF_TestDemo::getRandomStartGoal_JsonStr(unsigned num, std::mt19937 &MT) {
            using nlohmann::json;
            auto data = getRandomStartGoal(num, MT);
            json j;
            int i = 0;
            for (auto &item : data) {
                json o;
                o["idx"] = i;
                o["start"] = item.first;
                o["goal"] = item.second;
                j.push_back(o);
                i++;
            }
            return j.dump(4);
        }

        void MAPF_TestDemo::TestCaseParaGenerateDemo() { //get the demo json file;
            TestingFramework::MAPF::MAPF_TestDemo tester;
            auto j = tester.MT_Para_Demo();

            std::string str = j["case_path_root"].get<std::string>() + j["cases_list"][0].get<std::string>();
            tester.MapReader(str);
            auto raw_map = tester.RawMapReader(str);
            auto raw_task = tester.getRandomStartGoal_JsonStr((unsigned) 50,
                                                              TestingFramework::RandomHelper::GetCoreGenerator());

            nlohmann::json j2;
            j2["origin_case_name"] = j["cases_list"][0].get<std::string>();
            j2["case_path_root"] = j["case_path_root"].get<std::string>();
            j2["map"] = raw_map;
            j2["task"] = nlohmann::json::parse(raw_task);
            j2["seed"] = j["MR_list"][0]["testunit_seeds"][0].get<unsigned>();

            std::ofstream os;
            os.open(j["tmp_file_path_root"].get<std::string>() + "OriginTestCase1-para.json",
                    std::ios::out | std::ios::trunc);
            if (!os.is_open())
                LOG(ERROR) << "error~ can not find or create the file which named \" MT-para.json\"." << std::endl;
            os << (j2.dump(4));
            os.close();

            j2["seed"] = j["MR_list"][0]["testunit_seeds"][1].get<unsigned>();
            os.open(j["tmp_file_path_root"].get<std::string>() + "OriginTestCase2-para.json",
                    std::ios::out | std::ios::trunc);
            if (!os.is_open())
                LOG(ERROR) << "error~ can not find or create the file which named \" MT-para.json\"." << std::endl;
            os << (j2.dump(4));
            os.close();
        }

        json MAPF_TestDemo::MT_Para_Demo() {

            boost::filesystem::path log_path("./my_log.conf");
            if (boost::filesystem::exists(log_path)) {
                el::Configurations conf(log_path.string());
                el::Loggers::reconfigureAllLoggers(conf);
            } else {
                el::Configurations defaultConf;
                defaultConf.setToDefault();
                el::Loggers::reconfigureAllLoggers(defaultConf);
            }


            string timestamp = boost::posix_time::to_iso_string(boost::posix_time::second_clock::local_time());

            vector<unsigned> random_list;
            size_t repeat = 2;
            for (size_t i = 0; i < repeat; i++)
                random_list.push_back(RandomHelper::GetRandomInt());
            json j_para = OperRotationParam{
                    M_PI_2,
            };
            MR_Parameter mr_1{
                    boost::uuids::to_string(boost::uuids::random_generator()()),
                    "mr_1",
                    "",
                    "",
                    "Rotation",
                    random_list,
                    j_para.dump(),
            };

            random_list.clear();
            for (size_t i = 0; i < repeat; i++)
                random_list.push_back(RandomHelper::GetRandomInt());

            j_para = OperExchangeRandomParam{
                    5,
            };

            MR_Parameter mr_2{
                    boost::uuids::to_string(boost::uuids::random_generator()()),
                    "mr_2",
                    "",
                    "MT-Framework.so",
                    "ExchangeRandom",
                    random_list,
                    j_para.dump(),
            };

            random_list.clear();
            for (size_t i = 0; i < repeat; i++) {
                random_list.push_back(RandomHelper::GetRandomInt());
            }
            j_para = OperExchangeParam{};

            MR_Parameter mr_3{
                    boost::uuids::to_string(boost::uuids::random_generator()()),
                    "mr_3",
                    "",
                    "MT-Framework.so",
                    "Exchange",
                    random_list,
                    j_para.dump(),
            };

            random_list.clear();
            for (size_t i = 0; i < repeat; i++) {
                random_list.push_back(RandomHelper::GetRandomInt());
            }

            j_para = OperMirrorParam{
                    OperMirrorParam::horizon
            };

            MR_Parameter mr_4{
                    boost::uuids::to_string(boost::uuids::random_generator()()),
                    "mr_4",
                    "",
                    "MT-Framework.so",
                    "Mirror",
                    random_list,
                    j_para.dump(),
            };

            random_list.clear();
            for (size_t i = 0; i < repeat; i++) {
                random_list.push_back(RandomHelper::GetRandomInt());
            }


            j_para = OperPriorityExchangeParam{
                    {},
                    0.1, //default param
                    5,
            };


            MR_Parameter mr_5{
                    boost::uuids::to_string(boost::uuids::random_generator()()),
                    "mr_5",
                    "",
                    "MT-Framework.so",
                    "PriorityExchange",
                    random_list,
                    j_para.dump(),      //Exchange randomly
            };

            random_list.clear();
            for (size_t i = 0; i < repeat; i++)
                random_list.push_back(RandomHelper::GetRandomInt());

            j_para = OperRotationParam{
                    M_PI_2 * 3,
            };
            MR_Parameter mr_6{
                    boost::uuids::to_string(boost::uuids::random_generator()()),
                    "mr_6",
                    "",
                    "",
                    "Rotation",
                    random_list,
                    j_para.dump(),
            };


            random_list.clear();
            for (size_t i = 0; i < repeat; i++) {
                random_list.push_back(RandomHelper::GetRandomInt());
            }

            j_para = OperMirrorParam{
                    OperMirrorParam::vertical
            };
            MR_Parameter mr_7{
                    boost::uuids::to_string(boost::uuids::random_generator()()),
                    "mr_7",
                    "",
                    "MT-Framework.so",
                    "Mirror",
                    random_list,
                    j_para.dump(),
            };

            random_list.clear();
            for (size_t i = 0; i < repeat; i++) {
                random_list.push_back(RandomHelper::GetRandomInt());
            }

            j_para = OperDeleteAgentParam{
                    {},
                    0.2,
                    5,
            };

            MR_Parameter mr_8{
                    boost::uuids::to_string(boost::uuids::random_generator()()),
                    "mr_8",
                    "",
                    "MT-Framework.so",
                    "DeleteAgent",
                    random_list,
                    j_para.dump()};

            random_list.clear();
            for (size_t i = 0; i < repeat; i++) {
                random_list.push_back(RandomHelper::GetRandomInt());
            }

            j_para = OperDecreaseObstacleParam{
                    true,
                    false,
                    0.2,
                    5,
            };

            MR_Parameter mr_9{
                    boost::uuids::to_string(boost::uuids::random_generator()()),
                    "mr_9",
                    "",
                    "MT-Framework.so",
                    "DecreaseObstacle",
                    random_list,
                    j_para.dump()};

            random_list.clear();
            for (size_t i = 0; i < repeat; i++) {
                random_list.push_back(RandomHelper::GetRandomInt());
            }

            j_para = OperInceaseObstacleParam{
                    true,
                    false,
                    false,
                    false,
                    0.2,
                    5,
            };

            MR_Parameter mr_10{
                    boost::uuids::to_string(boost::uuids::random_generator()()),
                    "mr_10",
                    "",
                    "MT-Framework.so",
                    "IncreaseObstacle",
                    random_list,
                    j_para.dump()};

            //            MT_Parameter mt{
            //                    boost::str(boost::format{"This is a demo, created at %1%"} % timestamp),
            //                    repeat,
            //                    std::vector<MR_Parameter>{mr_1, mr_2, mr_3, mr_4, mr_5, mr_7, mr_8, mr_9},
            //                    //std::vector<MR_Parameter>{mr_10},
            //                    std::vector<std::string>{"arena.map", "warehouse_middle.map"},
            //                    "{\"ratio\" : 50 , \"shrink_mission_pos\" : 1}",
            //                    "/home/hiccup/Projects/ZXY-work/pibt/map/",
            //                    "NULL(no .so file)",
            //                    "/home/hiccup/Append/ZXY-Tmp/CollectedData",
            //                    "/home/hiccup/Append/ZXY-Tmp/"
            //            };

            //            MT_Parameter mt{
            //                    boost::str(boost::format{"This is a demo, created at %1%"} % timestamp),
            //                    repeat,
            //                    std::vector<MR_Parameter>{mr_1, mr_2, mr_3, mr_4, mr_5, mr_6},
            //                    //std::vector<MR_Parameter>{ mr_6},
            //                    std::vector<std::string>{"arena.map", "warehouse_middle.map"},
            //                    "{\"ratio\" : 0.1 , \"shrink_mission_pos\" : 1}",
            //                    "/home/hiccup/Projects/ZXY-work/pibt/map/",
            //                    "NULL(no .so file)",
            //                    "/home/hiccup/Append/ZXY-Tmp/CollectedData",
            //                    "/home/hiccup/Append/ZXY-Tmp/"
            //            };

            MT_Parameter mt{
                    boost::str(boost::format{"This is a demo, created at %1%"} % timestamp),
                    repeat,
                    std::vector<MR_Parameter>{mr_1, mr_2, mr_3, mr_4, mr_5, mr_6, mr_7, mr_8, mr_9},
                    //std::vector<MR_Parameter>{ mr_2},
                    std::vector<std::string>{"arena.map", "warehouse_middle.map", "beautiful8f.map",
                            //"den001d.map",
                                             "den009d.map",
                            //"den011d.map",
                                             "den101d.map", "den201d.map",
                                             "den202d.map", "den204d.map", "den207d.map", "den312d.map",
                            //"den520d.map",
                                             "empty-32-32.map"},
                    "{\"ratio\" : 50 , \"shrink_mission_pos\" : 0, \"algorithm\" : \"PIBT\"}",
                    "/home/hiccup/Projects/ZXY-work/pibt/map/",
                    "NULL(no .so file)",
                    "/home/hiccup/Append/ZXY-Tmp/CollectedData",
                    "/home/hiccup/Append/ZXY-Tmp/"};

            //            MT_Parameter mt{
            //                    boost::str(boost::format{"This is a demo, created at %1%"} % timestamp),
            //                    repeat,
            //                    std::vector<MR_Parameter>{mr_1, mr_2, mr_3, mr_4, mr_5, mr_6},
            //                    //std::vector<MR_Parameter>{ mr_6},
            //                    std::vector<std::string>{"den001d.map",
            //                            "den011d.map",
            //                            "den520d.map",
            //                                             },
            //                    "{\"ratio\" : 100 , \"shrink_mission_pos\" : 1}",
            //                    "/home/hiccup/Projects/ZXY-work/pibt/map/",
            //                    "NULL(no .so file)",
            //                    "/home/hiccup/Append/ZXY-Tmp/CollectedData",
            //                    "/home/hiccup/Append/ZXY-Tmp/"
            //            };

            //            MT_Parameter mt{
            //                    boost::str(boost::format{"This is a demo, created at %1%"} % timestamp),
            //                    repeat,
            //                    std::vector<MR_Parameter>{mr_1, mr_2, mr_3, mr_4, mr_5, mr_7, mr_9},
            //                    //std::vector<MR_Parameter>{mr_6},
            ////                    std::vector<std::string>{"Berlin_1_256.map", "Boston_0_256.map", "brc202d.map", "den312d.map",
            ////                                             "lak303d.map", "den520d.map", "empty-8-8.map", "empty-16-16.map",
            ////                                             "empty-32-32.map", "empty-48-48.map"},
            //                    std::vector<std::string>{"Berlin_1_256.map", "Boston_0_256.map"},
            //                    "{\"ratio\" : 1 , \"shrink_mission_pos\" : 1}",
            //                    "/home/hiccup/Projects/ZXY-work/MetamorphicTestingFramework/mpaf-map/",
            //                    "NULL(no .so file)",
            //                    "/home/hiccup/Append/ZXY-Tmp/CollectedData",
            //                    "/home/hiccup/Append/ZXY-Tmp/"
            //            };

            //            MT_Parameter mt{
            //                    boost::str(boost::format{"This is a demo, created at %1%"} % timestamp),
            //                    repeat,
            //                    std::vector<MR_Parameter>{mr_1, mr_2, mr_3, mr_4, mr_5, mr_7, mr_8, mr_9},
            //                    //std::vector<MR_Parameter>{mr_6},
            //                    std::vector<std::string>{"Berlin_1_256.map", "Boston_0_256.map", "brc202d.map", "den312d.map",
            //                                             "lak303d.map", "den520d.map",
            ////                                             "empty-8-8.map", "empty-16-16.map","empty-32-32.map", "empty-48-48.map"
            //                                             },
            //                    "{\"ratio\" : 100 , \"shrink_mission_pos\" : 1}",
            //                    "/home/hiccup/Projects/ZXY-work/MetamorphicTestingFramework/mpaf-map/",
            //                    "NULL(no .so file)",
            //                    "/home/hiccup/Append/ZXY-Tmp/CollectedData",
            //                    "/home/hiccup/Append/ZXY-Tmp/"
            //                };

            json j = mt;
            std::cout << j.dump(4) << std::endl;
            std::ofstream os;
            os.open(mt.tmp_file_path_root +
                    "MT-para.json",
                    std::ios::out | std::ios::trunc);
            if (!os.is_open())
                LOG(ERROR) << "error~ can not find or create the file which named \" MT-para.json\"." << std::endl;
            os << (j.dump(4));
            os.close();

            return j;
        }

        json MAPF_TestDemo::MT_Para_Demo2() {

            boost::filesystem::path log_path("./my_log.conf");
            if (boost::filesystem::exists(log_path)) {
                el::Configurations conf(log_path.string());
                el::Loggers::reconfigureAllLoggers(conf);
            } else {
                el::Configurations defaultConf;
                defaultConf.setToDefault();
                el::Loggers::reconfigureAllLoggers(defaultConf);
            }


            string timestamp = boost::posix_time::to_iso_string(boost::posix_time::second_clock::local_time());

            vector<unsigned> random_list;
            size_t repeat_unit = 20;
            string repeat_case = "10";
            for (size_t i = 0; i < repeat_unit; i++)
                random_list.push_back(RandomHelper::GetRandomInt());

            MR_Parameter mr_0{
                    boost::uuids::to_string(boost::uuids::random_generator()()),
                    "mr_0",
                    "",
                    "",
                    "RandomTesting",
                    random_list,
                    boost::str(boost::format{"{\"num\": %1%}"} % repeat_case),
            };


            MT_Parameter mt{
                    boost::str(boost::format{"This is a RandomTesting, created at %1%"} % timestamp),
                    repeat_unit,
                    std::vector<MR_Parameter>{mr_0},
                    //std::vector<MR_Parameter>{ mr_2},
                    std::vector<std::string>{"arena.map", "warehouse_middle.map", "beautiful8f.map",
                            //"den001d.map",
                                             "den009d.map",
                            //"den011d.map",
                                             "den101d.map", "den201d.map",
                                             "den202d.map", "den204d.map", "den207d.map", "den312d.map",
                            //"den520d.map",
                                             "empty-32-32.map"},
                    "{\"ratio\" : 30 ,\"shrink_mission_pos\" : 0, \"algorithm\" : \"PIBT\"}",
                    "/home/hiccup/Projects/ZXY-work/pibt/map/",
                    "NULL(no .so file)",
                    "/home/hiccup/Append/ZXY-Tmp/CollectedData",
                    "/home/hiccup/Append/ZXY-Tmp/"};

            json j = mt;
            std::cout << j.dump(4) << std::endl;
            std::ofstream os;
            os.open(mt.tmp_file_path_root +
                    "MT-para.json",
                    std::ios::out | std::ios::trunc);
            if (!os.is_open())
                LOG(ERROR) << "error~ can not find or create the file which named \" MT-para.json\"." << std::endl;
            os << (j.dump(4));
            os.close();

            return j;

        }
    } // namespace MAPF
} // namespace TestingFramework
