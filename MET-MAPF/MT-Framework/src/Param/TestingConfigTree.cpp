//
// Created by hiccup on 2020/5/3.
//

#include "../../include/Param/TestingConfigTree.h"
#include "../../include/MetamorphicTestingKernel.h"

#include <memory>

namespace TestingFramework {
//    std::function<vector<string>(MR_Parameter &mr_param, MT_Parameter &mt_param, string &map_name)>
//            MR_RelationGroup::origin_case_param_generator;
//    std::function<vector<string>(shared_ptr<IMetamorphicRelation> relation_ptr, string &origin_case_param)>
//            MR_TestUnit::follow_case_param_generator;

    PoolAlloc TestingFramework::PoolAlloc::single_instance;


    void TestingFramework::PoolAlloc::GabbageCollection() {
        boost::singleton_pool<boost::pool_allocator_tag, sizeof(MR_TestCase)>::release_memory();
        boost::singleton_pool<boost::pool_allocator_tag, sizeof(MR_TestUnit)>::release_memory();
        boost::singleton_pool<boost::pool_allocator_tag, sizeof(MR_RelationGroup)>::release_memory();
    }

    void TestingFramework::PoolAlloc::ReleaseMemory() {
        boost::singleton_pool<boost::pool_allocator_tag, sizeof(MR_TestCase)>::purge_memory();
        boost::singleton_pool<boost::pool_allocator_tag, sizeof(MR_TestUnit)>::purge_memory();
        boost::singleton_pool<boost::pool_allocator_tag, sizeof(MR_RelationGroup)>::purge_memory();
    }

    void MR_RelationGroup::generateRelationGroup(MR_Parameter &mr_param, MT_Parameter &mt_param) {

        LOG(INFO) << format{"Generate MR_RelationGroup with relation type: %1%"} % mr_param.name;

        relation_param = mr_param;
        auto j = json::parse(mr_param.MR_para_str);

        //relation init;
        relation_ptr = TestingFramework::MetamorphicTestingKernel::relationGeneratorFunc(mr_param);

        //each scenario with repeat num;
        for (auto &map_name : mt_param.cases_list) {
            //we need the IO part in mt_param;
            auto origin_params = relation_ptr->originCaseParamGenerator(mr_param, mt_param,
                                                                        map_name);
            if (origin_params.size() != mt_param.repeat_num) {
                LOG(ERROR) << format{"origin_params.size(): %1% !=  mt_param.repeat_num:%2%"}
                              % origin_params.size() % mt_param.repeat_num;
                exit(-2);
            }

            for (auto &origin_case_param : origin_params) {
                auto p = std::allocate_shared<MR_TestUnit>(PoolAlloc::single_instance.testunit_pool_alloc,
                                                           j["follow_cases"].size(),
                                                           this->relation_ptr);
                p->generateTestUnit(origin_case_param, mr_param, mt_param);
                unit_list.push_back(p);
            }
        }

        LOG(INFO) << format{"Generate relation type: %1% complete"} % mr_param.name;

    }

    void MR_ConfigTree::GenerateTree(MT_Parameter &mt_param) {

        LOG(INFO) << "Generate ConfigTree!";


        for (auto &mr_param : mt_param.MR_list) {
            auto p = std::allocate_shared<MR_RelationGroup>(PoolAlloc::single_instance.relationgroup_pool_alloc,
                                                            mt_param.repeat_num);
            p->generateRelationGroup(mr_param, mt_param);
            group_list.push_back(p);
        }
    }

    void MR_ConfigTree::GabbageCollection() {
        pool_alloc.GabbageCollection();
    }

    void MR_ConfigTree::Init_MR_ConfigTree(size_t MR_num) {
        group_list.reserve(MR_num);
    }

    void MR_TestUnit::generateTestUnit(string &origin_case_param, MR_Parameter &mr_param, MT_Parameter &mt_param) {
        json j = json::parse(origin_case_param);
        uuid_idx = j["uuid"].get<string>();
        seed = j["seed"].get<int>();
        VLOG(1) << format{"Generate TestUnit, uuid = %1%, seed = %2%"} % uuid_idx % seed;

//        //考虑到follow case的生成的输入由两部分组成，一部分为原case，一部分为case 的运行结果
//        //所以决定将follow up case的真正的参数生成挪到运行时，在此处仅提供内存开辟功能
//        //通过 followCaseNum 确定开辟的内存大小
//        //在运行时将真正的参数刷入
//        auto follow_case_num = relation_ptr->followCaseNum();
//        auto p = std::allocate_shared<MR_TestCase>(alloc.testcase_pool_alloc, origin_case_param, relation_ptr);
//        origin_case = p;
//
//        for (auto i =0 ; i< follow_case_num ;++i) {
//            //构造参数为空的follow_case_list， 运行时再将数据补齐
//            auto p = std::allocate_shared<MR_TestCase>(alloc.testcase_pool_alloc, "", relation_ptr);
//            follow_cases_list.push_back(p);
//        }

        //5.15.2020 最新版本，followCases 在 MAPF_ExecutionBase 的 lazyGenerateFollowUpTestCase 函数中创建，干掉followCaseNum函数
        auto p = std::allocate_shared<MR_TestCase>(PoolAlloc::single_instance.testcase_pool_alloc,
                                                   origin_case_param,
                                                   relation_ptr);
        origin_case = p;
    }

    void MR_TestCase::generateTestCaseParam() {
        json j = json::parse(this->input_args);
        auto map_name =  j["origin_map_name"].get<std::string>();
        VLOG(2) << format{"generateTestCaseParam uuid = %1%, map_name = %2%"} % uuid_idx % map_name;
        // 解析 cases 的仿真参数并塞进 磁盘中;
        // 这里由 Relation 提供行为
        relation_ptr->TestCaseParamGenerator(this->input_args);
    }

    MR_TestCase::MR_TestCase(const string &args, const shared_ptr<IMetamorphicRelation> &relationPtr)
            : input_args(args),
              relation_ptr(relationPtr) {
        json j = json::parse(args);
        uuid_idx = j["uuid"].get<string>();
        VLOG(1) << format{"Generating MR_TestCase info, uuid = %1%"} % uuid_idx;
    }
}

