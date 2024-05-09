//
// Created by hiccup on 2020/5/9.
//

#include "../../../include/MAPF/Execution/MAPF_ExecutionBase.h"
#include "../../../include/MAPF/Execution/MAPF_Execution.h"
#include "../../../include/MAPF/Relations/MAPF_MapHelper.h"
#include <boost/filesystem.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>


namespace TestingFramework {

//    std::function<void()> MAPF_ExecutionBase::testingCodeExecutionFunc;
//    std::function<string()> MAPF_ExecutionBase::originTestCaseResultReaderFunc;

    void MAPF_ExecutionBase::executeOriginCase(MR_TestUnit_ptr unit_ptr) {

        //using boost::gregorian;
        using boost::posix_time::ptime;
        using boost::posix_time::microsec_clock;

        auto idx = unit_ptr->origin_case->uuid_idx;
        //设定运行开始时间
        auto case_start_time = ptime(microsec_clock::universal_time());

        VLOG(2) << format{"Generate testUnit origin para files ,TestCase uuid = %1%"} % idx;

        unit_ptr->origin_case->generateTestCaseParam();

        VLOG(2) << format{"Start executing origin case ,TestCase uuid = %1%"} % idx;

        testingCodeExecutionFunc(unit_ptr->origin_case);

        auto case_end_time = ptime(microsec_clock::universal_time());

        DataCollectionPreprocessor(unit_ptr->origin_case, case_start_time, case_end_time);

        VLOG(2) << format{"Start collecting origin result ,TestCase uuid = %1%"} % idx;

        unit_ptr->origin_result = originTestCaseResultReaderFunc(unit_ptr->origin_case);

        VLOG(2) << "execution time:" << (case_end_time - case_start_time).total_microseconds();

        //通过观察实验现象可知，通常在超时之后，得到的轨迹的最后一个节点一般是一个异常值。
        //因为没有找到一个合适的 threshold 作为超时参数（场景大小，agent数量大小都会影响运行时间）
        //所以，暂时没有写 超时判定的部分，虽然实现起来并不难，但是超时边界难以确定。

        //TODO
        //Check legal execution
        //save to Case_ptr
        unit_ptr->origin_case->is_legal_execution = CheckExcecutionLegal(unit_ptr->origin_case);


        //read json ,hard code
        json o = json::parse( unit_ptr->origin_case->output_args);
        json o_in = json::parse( unit_ptr->origin_case->input_args);

        auto case_total_end_time = ptime(microsec_clock::universal_time());
        VLOG(2) << "Total time:" << (case_total_end_time - case_start_time).total_microseconds();
        o["total_microseconds_period"] = (case_total_end_time - case_start_time).total_microseconds();
        if(o_in.contains("ART_genterate_case_microseconds_period"))
            o["ART_genterate_case_microseconds_period"] = o_in["ART_genterate_case_microseconds_period"].get<int64_t>();
        unit_ptr->origin_case->output_args = o.dump(2);

        ProgressBarHelper::single_instance.showProgressBar();
    }

    void MAPF_ExecutionBase::DataCollectionPreprocessor
            (const MR_TestCase_ptr &case_ptr,
             const boost::posix_time::ptime &case_start_time,
             const boost::posix_time::ptime &case_end_time) {

        namespace fs = boost::filesystem;

        json o;
        o["case_start_time"] = to_iso_string(case_start_time);
        o["case_end_time"] = to_iso_string(case_end_time);
//        o["folder"] = boost::filesystem::current_path().string();
        json j = json::parse(case_ptr->input_args);
        fs::path p(j["param"]["para_file_name"].get<string>());
        // tmp_file_path_root;
        o["folder"] = p.parent_path().string();
        o["uuid"] = j["uuid"];

        this->data_collector_ptr->DataCollectionPreprocessor(o.dump(), case_ptr);
    }

    void MAPF_ExecutionBase::executeFollowCases(MR_TestUnit_ptr unit_ptr) {
        using boost::posix_time::ptime;
        using boost::posix_time::microsec_clock;
        // create follow up test cases;
        lazyGenerateFollowUpTestCase(unit_ptr);

        auto follow_cases = unit_ptr->follow_cases_list;

        for (auto &tmp_case :follow_cases) {
            VLOG(2) << format{"Start executing follow Case , TestCase uuid = %1%"} % tmp_case->uuid_idx;
            auto case_start_time = ptime(microsec_clock::universal_time());

            tmp_case->generateTestCaseParam();
            testingCodeExecutionFunc(tmp_case);

            auto case_end_time = ptime(microsec_clock::universal_time());

            DataCollectionPreprocessor(tmp_case, case_start_time, case_end_time);

            VLOG(2) << "execution time:" << (case_end_time - case_start_time).total_microseconds();
            //TODO
            //Check legal execution
            //save to Case_ptr
            tmp_case->is_legal_execution = CheckExcecutionLegal(tmp_case);

            auto case_total_end_time = ptime(microsec_clock::universal_time());
            json o = json::parse( tmp_case->output_args);
            o["total_microseconds_period"] = (case_total_end_time - case_start_time).total_microseconds();
            tmp_case->output_args = o.dump(2);

            ProgressBarHelper::single_instance.showProgressBar();
        }
    }

    void MAPF_ExecutionBase::lazyGenerateFollowUpTestCase(MR_TestUnit_ptr unit_ptr) {

        VLOG(2) << format{"Start init follow up cases parameter, TestUnit uuid = %1%"} % unit_ptr->uuid_idx;

        auto follow_case_param_list = unit_ptr->relation_ptr->followCaseParamGenerator(
                unit_ptr->origin_case->input_args,
                unit_ptr->origin_result);

        unit_ptr->follow_cases_list.reserve(follow_case_param_list.size());


        for (auto &follow_case_param : follow_case_param_list) {
            auto p = std::allocate_shared<MR_TestCase>(PoolAlloc::single_instance.testcase_pool_alloc,
                                                       follow_case_param, unit_ptr->relation_ptr);
            unit_ptr->follow_cases_list.push_back(p);
            VLOG(3) << format{"Generate follow up cases parameter, follow case uuid = %1%"} % p->uuid_idx;
        }

    }

    string MAPF_ExecutionBase::originTestCaseResultReaderFunc(MR_TestCase_ptr case_ptr) {

        //DataCollectionPreprocessor goes first, so case ptr output args has data.
        VLOG(3) << "@originTestCaseResultReaderFunc execution";
        return this->data_analyzer_ptr->ReadCaseResult(case_ptr);
    }

    bool MAPF_ExecutionBase::CheckExcecutionLegal(MR_TestCase_ptr case_ptr) {

#ifdef _SUT_VERSION_
        if (_SUT_VERSION_ == "2.0")      //检查是否碰到了 障碍物
        {
            return case_ptr->is_legal_execution;    //the program calculated the result
        }
#endif

//TODO update the new output format!!!!!
// Seems when ther result is not fit, it will be crushed.

        using nlohmann::json;
        auto j = json::parse(this->data_analyzer_ptr->ReadCaseResult(case_ptr));
        MAPF::PathsVector path_vect = j.get<MAPF::PathsVector>();
        auto j_input = json::parse(case_ptr->input_args);
//        std::cout<< j_input["map"]<<std::endl;

        string _map_data = j_input["map"].get<string>();

        MAPF::MapHelper helper;
        helper.MapReader(_map_data);

        //检查是否超过了允许的最大长度
        int overstep_limit = j_input["overstep_limit"].get<int>();
        if (std::any_of(path_vect.begin(), path_vect.end(),
                        [=](const MAPF::PathInfo &path_iter) {
                            bool result = (path_iter.path.size() >= overstep_limit);
                            if(result)
                            {
                                if (path_iter.path.size() < 20) {
                                    json j_path_iter = path_iter;
                                    VLOG(2) << "illegal path:" << j_path_iter.dump();
                                } else {
                                    auto path_part = vector<FPoint2>(path_iter.path.rbegin(), path_iter.path.rbegin() + 20);
                                    std::reverse(path_part.begin(), path_part.end());
                                    json j_path_iter = path_part;
                                    VLOG(2) << "illegal path(last 100):" << j_path_iter.dump();
                                }
                            }
                            return result;
                        })) {
            VLOG(2) << "There is a path which length is bigger than overtime_limits. Failed Case.";
            return false;
        }

        for (auto &path_iter : path_vect) {
            for (auto &pos : path_iter.path) {
                if (!helper.CheckPointSafe(pos)) {
                    VLOG(2) << "uuid" << case_ptr->uuid_idx;
                    if (path_iter.path.size() < 100) {
                        json j_path_iter = path_iter;
                        VLOG(2) << "illegal path:" << j_path_iter.dump();
                    } else {
                        auto path_part = vector<FPoint2>(path_iter.path.rbegin(), path_iter.path.rbegin() + 100);
                        std::reverse(path_part.begin(), path_part.end());
                        json j_path_iter = path_part;
                        VLOG(2) << "illegal path(last 100):" << j_path_iter.dump();
                    }
                    VLOG(2) << "illegal pos" << pos;
                    return false;
                }
            }
        }

        auto length = MAPF::RelationAnalyzeInfo::GetLength(path_vect);

        //check paths collision

        //检查是否互相碰到
        for (size_t i = 0; i < length; i++) {
            vector<FPoint2> tmp;

            for (auto &path_iter : path_vect) {
                if (path_iter.path.size() <= 1) {
                    LOG(WARNING) << "Illegal path size, only 1 or 0 zero point in the path";
                    json j = path_iter;
                    LOG(WARNING) << "Path information : \n" << j.dump() << endl;
                }

                FPoint2 pos;

                if (i < path_iter.path.size()) {
                    pos = path_iter.path[i];
                } else {
                    pos = *path_iter.path.crbegin(); //last one
                }

                if (tmp.size() >= 1) {
                    if (std::any_of(tmp.begin(), tmp.end(), [&](
                            const FPoint2 &_pos) -> bool { return _pos == pos; })) {
                        VLOG(2) << "=======================\n" << case_ptr->uuid_idx;
                        json j = tmp;
                        json j_p = path_iter;
                        json j_total = path_vect;
                        VLOG(2) << "tmp :" << j.dump();
                        VLOG(2) << "step :" << i;
                        VLOG(2) << "collision path:" << j_p.dump();
                        VLOG(2) << "paths :" << j_total.dump();
                        VLOG(2) << "=======================\n";
                        return false;
                    }

                } else {
                    tmp.push_back(pos);
                }
            }


        }

        return true;




    }


}