//
// Created by hiccup on 2020/5/2.
//

#include "../include/MetamorphicTestingKernel.h"


#include <iostream>
#include "nlohmann/json.hpp"
#include "boost/format.hpp"
#include "../include/Param/MT_Parameter.hpp"
#include "../include/RandomHelper.h"
#include "../include/MyDefine.hpp"
#include "boost/filesystem.hpp"
#include "../include/ART-Framework/ARTKernel.h"

namespace TestingFramework {
    std::function<MT_Relation_ptr(MR_Parameter &)> MetamorphicTestingKernel::relationGeneratorFunc;
//    std::function<void(int argc, char** argv)> MetamorphicTestingKernel::testingCodeExecutionFunc;
//    std::function<void()> MetamorphicTestingKernel::testingCodeExecutionFunc;
//    std::function<string()> MetamorphicTestingKernel::originTestCaseResultReaderFunc;


    MetamorphicTestingKernel::MetamorphicTestingKernel(string const &para_filename) {

        //std::cout<< boost::filesystem::current_path();
//        boost::filesystem::path log_path(para_filename);
//        log_path.parent_path() /= "my_log.conf";
//        if(boost::filesystem::exists(log_path))
//        {
//            el::Configurations conf(log_path.string());
//            el::Loggers::reconfigureAllLoggers(conf);
//        } else
//        {
//            el::Configurations defaultConf;
//            defaultConf.setToDefault();
//            el::Loggers::reconfigureLogger("default", defaultConf);
//        }


        LOG(TRACE) << "***** trace log init *****";
        LOG(DEBUG) << "***** debug log init *****";
        LOG(ERROR) << "***** error log init *****";
        LOG(WARNING) << "***** warning log init *****";
        LOG(INFO) << "***** info log init *****";
        VLOG(1) << "***** verbose v1 log init *****";

        RandomHelper::InitSeed();

        readPara(para_filename); //update _mt_param;
        _execute_ptr = ObjPool::allocate<MAPF::MAPF_Execution>(_mt_param.data_path_root);

        _config_tree.Init_MR_ConfigTree(this->_mt_param.MR_list.size());


    }

    void MetamorphicTestingKernel::readPara(const string &para_filename) {
        using json = nlohmann::json;
        using boost::format;

        //多次使用前还是先清理一下的吧;
        _mt_param.MR_list.clear();
        _mt_param.cases_list.clear();

        LOG(INFO) << format{"start reading %1%"} % para_filename;

        std::ifstream fs;
        json jf;

        try {
            fs.open(para_filename);
            if (!fs.is_open())
                LOG(ERROR) << format{"[error]: can not open file: %1%."} % para_filename;
            fs >> jf;
            this->_mt_param = jf;
            fs.close();
        }
        catch (...) {
            LOG(ERROR) << format{"[error]: string 2 json converting failed: %1%."} % para_filename;
            fs.close();
            exit(-1);
        }

        _mt_param = jf;
        if (!checkParam(_mt_param)) {
            LOG(ERROR) << "[error]: Metamorphic Testing parameter illegal. Force to terminated." << std::endl;
            exit(-1);
        }

        LOG(INFO) << format{"reading parameter file %1% complete!"} % para_filename;
    }


    void MetamorphicTestingKernel::initTesting(std::function<MT_Relation_ptr(MR_Parameter &)> relation_generator) {
        relationGeneratorFunc = relation_generator;

        _config_tree.GenerateTree(_mt_param);

    }


    void MetamorphicTestingKernel::execution() {

        unsigned n = 0;
        for (auto &mr_group  : this->_config_tree.group_list) {
//            for (auto &test_unit : mr_group->unit_list) {
//                ++n;
//            }
            n += mr_group->unit_list.size();
        }

        ProgressBarHelper::single_instance.setTotalNum(n);
        ProgressBarHelper::single_instance.updateProgressBar();

        SaveMTparam();
        sleep(1);

        for (auto &mr_group  : this->_config_tree.group_list) {
            for (auto &test_unit : mr_group->unit_list) {

                _execute_ptr->executeOriginCase(test_unit);
                _execute_ptr->executeFollowCases(test_unit);
                test_unit->relation_satisfied_record =
                        _execute_ptr->data_analyzer_ptr->UnitAnalysisRecording(test_unit);
                ProgressBarHelper::single_instance.updateProgressBar();
            }
        }
    }


    void MetamorphicTestingKernel::executionART(ARTTestingFramework::ARTKernelParam ART_kernel_param) {

        unsigned n = 0;
        for (auto &mr_group  : this->_config_tree.group_list) {
            n += mr_group->unit_list.size();
        }

        ProgressBarHelper::single_instance.setTotalNum(n);
        ProgressBarHelper::single_instance.updateProgressBar();

        SaveMTparam();

        for (auto &mr_group  : this->_config_tree.group_list) {
            ARTTestingFramework::ARTKernel ART_kernel =
                    ARTTestingFramework::ARTKernel(ART_kernel_param, mr_group->relation_param);

            for (auto &test_unit : mr_group->unit_list) {

//                //生成 一些新的origin case? 随时准备替换？
//                auto mr_param = mr_group->relation_param;
//                auto j = json::parse(mr_param.MR_para_str);
//                auto relation_ptr = TestingFramework::MetamorphicTestingKernel::relationGeneratorFunc(mr_param);
//                json o_arg = json::parse(test_unit->origin_case->input_args);
//                auto map_name = o_arg["origin_map_name"].get<std::string>();
//                auto origin_params = relation_ptr->originCaseParamGenerator(mr_param,_mt_param,map_name);
//
//                //选择最优的 case
//                auto origin_case_param = origin_params[0];
//                auto p = std::allocate_shared<MR_TestUnit>(PoolAlloc::single_instance.testunit_pool_alloc,
//                                                           j["follow_cases"].size(),
//                                                           relation_ptr);
//                p->generateTestUnit(origin_case_param, mr_param, _mt_param);
//                test_unit =p;


                test_unit = ART_kernel.generateNextTestingUnit(test_unit);


                _execute_ptr->executeOriginCase(test_unit);
                _execute_ptr->executeFollowCases(test_unit);
                test_unit->relation_satisfied_record =
                        _execute_ptr->data_analyzer_ptr->UnitAnalysisRecording(test_unit);

                if (std::all_of(test_unit->follow_cases_list.begin(), test_unit->follow_cases_list.end(),
                                [&](std::shared_ptr<MR_TestCase> c) { return c->is_legal_execution == true; })
                    || test_unit->origin_case->is_legal_execution == true)
                    ART_kernel.IsTestUnitSuccess(true,test_unit);

                ProgressBarHelper::single_instance.updateProgressBar();
            }
        }


    }


    void MetamorphicTestingKernel::SaveMTparam() const {//copy MT-param;
        string para_file_name = "MT-para.json";
        auto &path = _execute_ptr->data_collector_ptr->target_folder_path;
        std::ofstream os;
        os.open(path + para_file_name, std::ios::out | std::ios::trunc);
        if (!os.is_open())
            LOG(ERROR) << format{"error~ can not find or create the file : %1%"} % para_file_name;
        os << MT_param_str << std::endl;
        os.close();
    }


    bool MetamorphicTestingKernel::checkParam(MT_Parameter &param) {
        //TODO
        return true;
    }

    void MetamorphicTestingKernel::collectResultInfo() {
        this->_execute_ptr->data_collector_ptr->DataCollection(this->_config_tree);
    }

    void MetamorphicTestingKernel::analyzeResultInfo() {
        this->_execute_ptr->data_analyzer_ptr->ArrangeAnalysisRecord(this->_config_tree);
    }

    MR_ConfigTree MetamorphicTestingKernel::getConfigTree() {
        return this->_config_tree;
    }
}

