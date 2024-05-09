//
// Created by hiccup on 2020/9/10.
//

#include "MR_ParameterGenerator.h"

#include "nlohmann/json.hpp"

#include "include/RandomHelper.h"
#include "include/MAPF/Operator/MAPF_MissionParam.hpp"
#include "include/MAPF/Operator/MAPF_CoordinateParam.hpp"
#include "include/MAPF/Operator/MAPF_InterativeParam.hpp"
#include "include/MAPF/Operator/MAPF_ObstacleParam.hpp"


using namespace TestingFramework;
using namespace TestingFramework::MAPF;

void MR_ParameterGenerator::seed_setting(std::vector<MR_Parameter> &param_list) {
    if (!random_list.empty())
        test_unit_num = random_list.size();
    else {
        for (size_t i = 0; i < test_unit_num; i++)
            random_list.push_back(TestingFramework::RandomHelper::GetRandomInt());
    }

    for (auto &item : param_list) {
        if (total_random) {
            random_list.clear();
            for (size_t i = 0; i < test_unit_num; i++)
                random_list.push_back(TestingFramework::RandomHelper::GetRandomInt());
            item.testunit_seeds = random_list;
        } else {
            item.testunit_seeds = random_list;
        }

    }


}



std::vector<MR_Parameter> MR_ParameterGenerator::getVector() {

    //# lv0 Environment Layer

    nlohmann::json j_para = OperRotationParam{
            M_PI_2,
    };
    TestingFramework::MR_Parameter rotation_1{
            boost::uuids::to_string(boost::uuids::random_generator()()),
            "mr_1-p1",
            "",
            "",
            "Rotation",
            random_list,
            j_para.dump(),
    };


    j_para = OperRotationParam{
            M_PI_2 * 2,
    };
    TestingFramework::MR_Parameter rotation_2{
            boost::uuids::to_string(boost::uuids::random_generator()()),
            "mr_1-p2",
            "",
            "",
            "Rotation",
            random_list,
            j_para.dump(),
    };


    j_para = OperRotationParam{
            M_PI_2 * 3,
    };
    TestingFramework::MR_Parameter rotation_3{
            boost::uuids::to_string(boost::uuids::random_generator()()),
            "mr_1-p3",
            "",
            "",
            "Rotation",
            random_list,
            j_para.dump(),
    };

//    ===============================================

    j_para = OperMirrorParam{
            OperMirrorParam::horizon
    };
    TestingFramework::MR_Parameter mirror_1{
            boost::uuids::to_string(boost::uuids::random_generator()()),
            "mr_2-p1",
            "",
            "MT-Framework.so",
            "Mirror",
            random_list,
            j_para.dump(),
    };

    j_para = OperMirrorParam{
            OperMirrorParam::vertical
    };
    TestingFramework::MR_Parameter mirror_2{
            boost::uuids::to_string(boost::uuids::random_generator()()),
            "mr_2-p2",
            "",
            "MT-Framework.so",
            "Mirror",
            random_list,
            j_para.dump(),
    };

    j_para = OperMirrorParam{
            OperMirrorParam::center
    };
    TestingFramework::MR_Parameter mirror_3{
            boost::uuids::to_string(boost::uuids::random_generator()()),
            "mr_2-p3",
            "",
            "MT-Framework.so",
            "Mirror",
            random_list,
            j_para.dump(),
    };

    // ========================================================

    j_para = OperDecreaseObstacleParam{
            true,
            false,
            0.2,
            1,
    };

    TestingFramework::MR_Parameter decrease_obstacle{
            boost::uuids::to_string(boost::uuids::random_generator()()),
            "mr_3-p1",
            "",
            "MT-Framework.so",
            "DecreaseObstacle",
            random_list,
            j_para.dump()};

    j_para = OperDecreaseObstacleParam{
            true,
            true,
            0.2,
            1,
    };
    TestingFramework::MR_Parameter decrease_obstacle_erosion_random{
            boost::uuids::to_string(boost::uuids::random_generator()()),
            "mr_3-p2",
            "",
            "MT-Framework.so",
            "DecreaseObstacle",
            random_list,
            j_para.dump()};

    j_para = OperDecreaseObstacleParam{
            false,
            true,
            0.2,
            1,
    };

    TestingFramework::MR_Parameter decrease_obstacle_erosion_total{
            boost::uuids::to_string(boost::uuids::random_generator()()),
            "mr_3-p3",
            "",
            "MT-Framework.so",
            "DecreaseObstacle",
            random_list,
            j_para.dump()};

    //============================================================================

    j_para = OperInceaseObstacleParam{
            true,
            false,
            false,
            false,
            0.05,
            1,
    };

    TestingFramework::MR_Parameter increase_obstacle_random_1{
            boost::uuids::to_string(boost::uuids::random_generator()()),
            "mr_4.1-p1",
            "",
            "MT-Framework.so",
            "IncreaseObstacleRandom",
            random_list,
            j_para.dump()};

    j_para = OperInceaseObstacleParam{
            true,
            false,
            false,
            false,
            0.1,
            1,
    };

    TestingFramework::MR_Parameter increase_obstacle_random_2{
            boost::uuids::to_string(boost::uuids::random_generator()()),
            "mr_4.1-p2",
            "",
            "MT-Framework.so",
            "IncreaseObstacleRandom",
            random_list,
            j_para.dump()};

    j_para = OperInceaseObstacleParam{
            true,
            false,
            false,
            false,
            0.15,
            1,
    };

    TestingFramework::MR_Parameter increase_obstacle_random_3{
            boost::uuids::to_string(boost::uuids::random_generator()()),
            "mr_4.1-p3",
            "",
            "MT-Framework.so",
            "IncreaseObstacleRandom",
            random_list,
            j_para.dump()};

    //=============================================================================

    //# lv1 Single-Agent Layer

//    j_para = OperInceaseObstacleParam{
//            true,
//            false,
//            true,
//            false,
//            0.05,
//            1,
//    };
//
//    TestingFramework::MR_Parameter increase_obstacle_noninteraction{
//            boost::uuids::to_string(boost::uuids::random_generator()()),
//            "mr_4-p2",
//            "",
//            "MT-Framework.so",
//            "IncreaseObstacle",
//            random_list,
//            j_para.dump()};
//
//
//    j_para = OperInceaseObstacleParam{
//            false,
//            true,
//            false,
//            false,
//            0.05,
//            1,
//    };
//
//    TestingFramework::MR_Parameter increase_obstacle_interaction{
//            boost::uuids::to_string(boost::uuids::random_generator()()),
//            "mr_4-p3",
//            "",
//            "MT-Framework.so",
//            "IncreaseObstacle",
//            random_list,
//            j_para.dump()};

    j_para = OperInceaseObstacleParam{
            true,
            false,
            true,
            false,
            0.05,
            1,
    };

    TestingFramework::MR_Parameter increase_obstacle_noninteraction_1{
            boost::uuids::to_string(boost::uuids::random_generator()()),
            "mr_4.2-p1",
            "",
            "MT-Framework.so",
            "IncreaseObstacleNoninteraction",
            random_list,
            j_para.dump()};

    j_para = OperInceaseObstacleParam{
            true,
            false,
            true,
            false,
            0.1,
            1,
    };

    TestingFramework::MR_Parameter increase_obstacle_noninteraction_2{
            boost::uuids::to_string(boost::uuids::random_generator()()),
            "mr_4.2-p2",
            "",
            "MT-Framework.so",
            "IncreaseObstacleNoninteraction",
            random_list,
            j_para.dump()};

    j_para = OperInceaseObstacleParam{
            true,
            false,
            true,
            false,
            0.15,
            1,
    };

    TestingFramework::MR_Parameter increase_obstacle_noninteraction_3{
            boost::uuids::to_string(boost::uuids::random_generator()()),
            "mr_4.2-p3",
            "",
            "MT-Framework.so",
            "IncreaseObstacleNoninteraction",
            random_list,
            j_para.dump()};

    //============================================================================


    j_para = OperExchangeParam{
            false,
    };
    TestingFramework::MR_Parameter exchange_1{
            boost::uuids::to_string(boost::uuids::random_generator()()),
            "mr_5-p1",
            "",
            "MT-Framework.so",
            "Exchange",
            random_list,
            j_para.dump(),
    };

    j_para = OperExchangeParam{
            true,
    };
    TestingFramework::MR_Parameter exchange_2{
            boost::uuids::to_string(boost::uuids::random_generator()()),
            "mr_5-p2",
            "",
            "MT-Framework.so",
            "Exchange",
            random_list,
            j_para.dump(),
    };

    j_para = OperExchangeParam{
            true,
    };
    TestingFramework::MR_Parameter exchange_3{
            boost::uuids::to_string(boost::uuids::random_generator()()),
            "mr_5-p3",
            "",
            "MT-Framework.so",
            "Exchange",
            random_list,
            j_para.dump(),
    };


    j_para = OperMoveAgentParam{
            0.1,
            1,
            TestingFramework::MAPF::OperMoveAgentParam::START,
    };

    MR_Parameter shrink_start_pos_1{
            boost::uuids::to_string(boost::uuids::random_generator()()),
            "mr_6-p1",
            "ratio = 0.1 ",
            "",
            "ShrinkStartPos",
            random_list,
            j_para.dump()

    };


    j_para = OperMoveAgentParam{
            0.2,
            1,
            TestingFramework::MAPF::OperMoveAgentParam::START,
    };

    MR_Parameter shrink_start_pos_2{
            boost::uuids::to_string(boost::uuids::random_generator()()),
            "mr_6-p2",
            "ratio = 0.2 ",
            "",
            "ShrinkStartPos",
            random_list,
            j_para.dump()

    };

    j_para = OperMoveAgentParam{
            0.3,
            1,
            TestingFramework::MAPF::OperMoveAgentParam::START,
    };

    MR_Parameter shrink_start_pos_3{
            boost::uuids::to_string(boost::uuids::random_generator()()),
            "mr_6-p3",
            "ratio = 0.3 ",
            "",
            "ShrinkStartPos",
            random_list,
            j_para.dump()

    };



    //# lv3 Multi-Agent Interaction Layer

    j_para = OperPriorityExchangeParam{
            {},
            0.1, //default param
            1,
    };
    TestingFramework::MR_Parameter priority_exchange_1{
            boost::uuids::to_string(boost::uuids::random_generator()()),
            "mr_7-p1",
            "",
            "MT-Framework.so",
            "PriorityExchange",
            random_list,
            j_para.dump(),      //Exchange randomly
    };

    j_para = OperPriorityExchangeParam{
            {},
            0.2, //default param
            1,
    };
    TestingFramework::MR_Parameter priority_exchange_2{
            boost::uuids::to_string(boost::uuids::random_generator()()),
            "mr_7-p2",
            "",
            "MT-Framework.so",
            "PriorityExchange",
            random_list,
            j_para.dump(),      //Exchange randomly
    };

    j_para = OperPriorityExchangeParam{
            {},
            0.3, //default param
            1,
    };
    TestingFramework::MR_Parameter priority_exchange_3{
            boost::uuids::to_string(boost::uuids::random_generator()()),
            "mr_7-p3",
            "",
            "MT-Framework.so",
            "PriorityExchange",
            random_list,
            j_para.dump(),      //Exchange randomly
    };


    j_para = OperDeleteAgentParam{
            {},
            0.1,
            1,
    };

    TestingFramework::MR_Parameter delete_agent_1{
            boost::uuids::to_string(boost::uuids::random_generator()()),
            "mr_8-p1",
            "",
            "MT-Framework.so",
            "DeleteAgent",
            random_list,
            j_para.dump()};

    j_para = OperDeleteAgentParam{
            {},
            0.2,
            1,
    };

    TestingFramework::MR_Parameter delete_agent_2{
            boost::uuids::to_string(boost::uuids::random_generator()()),
            "mr_8-p2",
            "",
            "MT-Framework.so",
            "DeleteAgent",
            random_list,
            j_para.dump()};

    j_para = OperDeleteAgentParam{
            {},
            0.3,
            1,
    };

    TestingFramework::MR_Parameter delete_agent_3{
            boost::uuids::to_string(boost::uuids::random_generator()()),
            "mr_8-p3",
            "",
            "MT-Framework.so",
            "DeleteAgent",
            random_list,
            j_para.dump()};


    j_para = OperAddAgentParam{
            0.1,
            1,
            false,
            {},
            {},
    };

    MR_Parameter add_agent_1{
            boost::uuids::to_string(boost::uuids::random_generator()()),
            "mr_9-p1",
            "",
            "",
            "AddAgent",
            random_list,
            j_para.dump()
    };

    j_para = OperAddAgentParam{
            0.2,
            1,
            false,
            {},
            {},
    };

    MR_Parameter add_agent_2{
            boost::uuids::to_string(boost::uuids::random_generator()()),
            "mr_9-p2",
            "",
            "",
            "AddAgent",
            random_list,
            j_para.dump()
    };
    j_para = OperAddAgentParam{
            0.3,
            1,
            false,
            {},
            {},
    };

    MR_Parameter add_agent_3{
            boost::uuids::to_string(boost::uuids::random_generator()()),
            "mr_9-p3",
            "",
            "",
            "AddAgent",
            random_list,
            j_para.dump()
    };




    //lv3

    j_para = OperAddAgentParam{
            0.1,
            1,
            true,
            {},
            {},
    };

    MR_Parameter random_probe_nonintersection_1{
            boost::uuids::to_string(boost::uuids::random_generator()()),
            "mr_11-p1",
            "",
            "",
            "AddRandomPriorityProbe",
            random_list,
            j_para.dump()
    };

    j_para = OperAddAgentParam{
            0.2,
            1,
            true,
            {},
            {},
    };

    MR_Parameter random_probe_nonintersection_2{
            boost::uuids::to_string(boost::uuids::random_generator()()),
            "mr_11-p2",
            "",
            "",
            "AddRandomPriorityProbe",
            random_list,
            j_para.dump()
    };

    j_para = OperAddAgentParam{
            0.3,
            1,
            true,
            {},
            {},
    };

    MR_Parameter random_probe_nonintersection_3{
            boost::uuids::to_string(boost::uuids::random_generator()()),
            "mr_11-p3",
            "",
            "",
            "AddRandomPriorityProbe",
            random_list,
            j_para.dump()
    };


    j_para = OperAddAgentParam{
            0.1,
            1,
            true,
            {},
            {},
            false,
            3
    };

    MR_Parameter window_probe_v1{
            boost::uuids::to_string(boost::uuids::random_generator()()),
            "mr_12-p1",
            "window = 1",
            "",
            "AddWindowedPriorityProbe",
            random_list,
            j_para.dump()
    };

    j_para = OperAddAgentParam{
            0.2,
            1,
            true,
            {},
            {},
            false,
            3
    };

    MR_Parameter window_probe_v2{
            boost::uuids::to_string(boost::uuids::random_generator()()),
            "mr_12-p2",
            "window = 3",
            "",
            "AddWindowedPriorityProbe",
            random_list,
            j_para.dump()
    };

    j_para = OperAddAgentParam{
            0.3,
            1,
            true,
            {},
            {},
            false,
            3
    };
    MR_Parameter window_probe_v3{
            boost::uuids::to_string(boost::uuids::random_generator()()),
            "mr_12-p3",
            "window = 5",
            "",
            "AddWindowedPriorityProbe",
            random_list,
            j_para.dump()
    };


    std::vector<MR_Parameter> mr_para_vect = {};
    std::vector<MR_Parameter> lv1 = {rotation_1, rotation_2, rotation_3,

                                     mirror_1, mirror_2, mirror_3,

                                     decrease_obstacle,
                                     decrease_obstacle_erosion_random,
                                     decrease_obstacle_erosion_total,
                                     increase_obstacle_random_1,
                                     increase_obstacle_random_2,
                                     increase_obstacle_random_3
    };


    std::vector<MR_Parameter> lv2 = {
            increase_obstacle_noninteraction_1,
            increase_obstacle_noninteraction_2,
            increase_obstacle_noninteraction_3,
            exchange_1, exchange_2, exchange_3,
            shrink_start_pos_1, shrink_start_pos_2, shrink_start_pos_3};


    std::vector<MR_Parameter> lv3 = {priority_exchange_1, priority_exchange_2, priority_exchange_3,
                                     add_agent_1, add_agent_2, add_agent_3,
                                     delete_agent_1, delete_agent_2, delete_agent_3,
                                     window_probe_v1,
                                     window_probe_v2,
                                     window_probe_v3,
                                     random_probe_nonintersection_1,
                                     random_probe_nonintersection_2,
                                     random_probe_nonintersection_3
    };

    std::vector<MR_Parameter> added = {increase_obstacle_random_1,
                                       increase_obstacle_random_2,
                                       increase_obstacle_random_3,
                                       increase_obstacle_noninteraction_1,
                                       increase_obstacle_noninteraction_2,
                                       increase_obstacle_noninteraction_3,
                                       random_probe_nonintersection_1,
                                       random_probe_nonintersection_2,
                                       random_probe_nonintersection_3};

//    std::vector<MR_Parameter> tested = {shrink_start_pos_1, shrink_start_pos_2, shrink_start_pos_3};
//    std::move(tested.begin(), tested.end(), std::inserter(mr_para_vect, mr_para_vect.end()));

    std::move(lv1.begin(), lv1.end(), std::inserter(mr_para_vect, mr_para_vect.end()));
    std::move(lv2.begin(), lv2.end(), std::inserter(mr_para_vect, mr_para_vect.end()));
    std::move(lv3.begin(), lv3.end(), std::inserter(mr_para_vect, mr_para_vect.end()));

    seed_setting(mr_para_vect);

    return mr_para_vect;
}

std::vector<MR_Parameter> MR_ParameterGenerator::getRandomVector() {


    auto base_mr_para_vect = getVector();
    auto mr_para_vect = std::vector<MR_Parameter>();
    //默认每个mr 对应三个param，然后3选1
    const int param_const = 3;
    mr_para_vect.reserve(base_mr_para_vect.size()/param_const);


    for(int i =0 ;i < base_mr_para_vect.size() / 3 ; ++i)
    {
        auto selection_idx = RandomHelper::GetRandomInt() % 3;
        auto selcted_item = base_mr_para_vect[i*3 + selection_idx];
        vector<string> strs;
        boost::split(strs,selcted_item.name,boost::is_any_of("-"));
//        selcted_item.name = strs[0] + "-" + "p0";
        mr_para_vect.push_back(selcted_item);
    }

    return mr_para_vect;
}
