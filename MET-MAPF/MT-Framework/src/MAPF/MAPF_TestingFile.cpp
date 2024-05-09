//
// Created by hiccup on 2020/7/28.
//

#include "../../include/MAPF/MAPF_TestingFile.h"
#include "boost/filesystem.hpp"

namespace TestingFramework {
    namespace MAPF {

        json MAPF_TestingFile::RandomTestingFileGenerator(const vector<unsigned> &random_list) {
            string timestamp = boost::posix_time::to_iso_string(boost::posix_time::second_clock::local_time());

//            for (size_t i = 0; i < repeat_unit; i++)
//                random_list.push_back(RandomHelper::GetRandomInt());

            MR_Parameter mr_0{
                    boost::uuids::to_string(boost::uuids::random_generator()()),
                    "mr_0",
                    "",
                    "",
                    "RandomTesting",
                    random_list,
                    boost::str(boost::format{"{\"num\": %1%,\"random_seed\": %2%}"} % repeat_case % "true"),
            };

            json j_param = para;
            MT_Parameter mt{
                    boost::str(boost::format{"This is a RandomTesting, created at %1%"} % timestamp),
                    repeat_unit,
                    std::vector<MR_Parameter>{mr_0},
                    //std::vector<MR_Parameter>{ mr_2},
                    maps,
                    j_param.dump(),
                    case_path_root,
                    "NULL(no .so file)",
                    data_path_root,
                    tmp_file_path_root};
            {
                namespace fs = boost::filesystem;
                if (!(fs::exists(data_path_root) && fs::is_directory(data_path_root))) {
                    LOG(WARNING) << format{"Path : '%1%' is not exist"} % data_path_root;
                    fs::create_directories(data_path_root);
                    sleep(1);
                }
            }


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

        json MAPF_TestingFile::MetamorphicTestingFileGenerator(std::vector<MR_Parameter> &param_vect) {
            string timestamp = boost::posix_time::to_iso_string(boost::posix_time::second_clock::local_time());


            json j_param = para;
            MT_Parameter mt{
                    boost::str(boost::format{"This is a metamorphic testing, created at %1%"} % timestamp),
                    repeat_unit,
                    param_vect,
                    maps,
                    j_param.dump(),
                    case_path_root,
                    "NULL(no .so file)",
                    data_path_root,
                    tmp_file_path_root};
            {
                namespace fs = boost::filesystem;
                if (!(fs::exists(data_path_root) && fs::is_directory(data_path_root))) {
                    LOG(WARNING) << format{"Path : '%1%' is not exist"} % data_path_root;
                    fs::create_directories(data_path_root);
                    sleep(1);
                }
            }


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
    }
};

