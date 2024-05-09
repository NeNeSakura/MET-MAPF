//
// Created by hiccup on 2022/8/16.
//

#include "MR_ParameterGeneratorV2.h"
#include "nlohmann/json.hpp"

#include "include/RandomHelper.h"
#include "include/MAPF/Operator/MAPF_MissionParam.hpp"
#include "include/MAPF/Operator/MAPF_CoordinateParam.hpp"
#include "include/MAPF/Operator/MAPF_InterativeParam.hpp"
#include "include/MAPF/Operator/MAPF_ObstacleParam.hpp"

using namespace TestingFramework;
using namespace TestingFramework::MAPF;

 std::vector<MR_Parameter> MR_ParameterGeneratorV2::getVector()  {

     //# lv1 Environment Layer

     nlohmann::json j_para = OperMirrorParam{
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


     // ========================================================

     j_para = OperDecreaseObstacleParam{
             true,
             false,
             0.1,
             1,
     };
     TestingFramework::MR_Parameter decrease_obstacle_erosion_random1{
             boost::uuids::to_string(boost::uuids::random_generator()()),
             "mr_3-p1",
             "",
             "MT-Framework.so",
             "DecreaseObstacle",
             random_list,
             j_para.dump()};

     j_para = OperDecreaseObstacleParam{
             true,
             false,
             0.2,
             1,
     };
     TestingFramework::MR_Parameter decrease_obstacle_erosion_random2{
             boost::uuids::to_string(boost::uuids::random_generator()()),
             "mr_3-p2",
             "",
             "MT-Framework.so",
             "DecreaseObstacle",
             random_list,
             j_para.dump()};

     // ========================================================

     j_para = OperInceaseObstacleParam{
             true,
             false,
             false,
             true,
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
             true,
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

     // ========================================================

     //# lv2 Single Agent Layer

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

     // ========================================================

     j_para = OperExchangeParam{
             false,
     };
     TestingFramework::MR_Parameter swap_1{
             boost::uuids::to_string(boost::uuids::random_generator()()),
             "mr_5-p1",
             "",
             "MT-Framework.so",
             "Exchange",
             random_list,
             j_para.dump(),
     };

//     j_para = OperExchangeParam{
//             true,
//     };
//     TestingFramework::MR_Parameter swap_2{
//             boost::uuids::to_string(boost::uuids::random_generator()()),
//             "mr_5-p2",
//             "",
//             "MT-Framework.so",
//             "Exchange",
//             random_list,
//             j_para.dump(),
//     };

     // =============================================================

     //shorten
     j_para = OperMoveAgentParam{
             0.1,
             1,
             TestingFramework::MAPF::OperMoveAgentParam::START,
     };

     MR_Parameter shorten_1{
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

     MR_Parameter shorten_2{
             boost::uuids::to_string(boost::uuids::random_generator()()),
             "mr_6-p2",
             "ratio = 0.2 ",
             "",
             "ShrinkStartPos",
             random_list,
             j_para.dump()

     };

     // =============================================================

     //# lv3 Multi-Agent Interaction Layer


     j_para = OperPriorityExchangeParam{
             {},
             0.1, //default param
             1,
     };
     TestingFramework::MR_Parameter priority_swap_1{
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
     TestingFramework::MR_Parameter priority_swap_2{
             boost::uuids::to_string(boost::uuids::random_generator()()),
             "mr_7-p2",
             "",
             "MT-Framework.so",
             "PriorityExchange",
             random_list,
             j_para.dump(),      //Exchange randomly
     };

     // =============================================================

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

     // ========================================================

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



     // ========================================================

     j_para = OperAddAgentParam{
             0.1,
             1,
             true,
             {},
             {},
             false,
             2
     };

     MR_Parameter window_probe_v1{
             boost::uuids::to_string(boost::uuids::random_generator()()),
             "mr_12-p1",
             "window = 2",
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
             2
     };

     MR_Parameter window_probe_v2{
             boost::uuids::to_string(boost::uuids::random_generator()()),
             "mr_12-p2",
             "window = 2",
             "",
             "AddWindowedPriorityProbe",
             random_list,
             j_para.dump()
     };

     // ========================================================
     std::vector<MR_Parameter> lv1 ={mirror_1,mirror_2,
                                     decrease_obstacle_erosion_random1,decrease_obstacle_erosion_random2,
                                     increase_obstacle_random_1,increase_obstacle_random_2};
     std::vector<MR_Parameter> lv2 ={increase_obstacle_noninteraction_1, increase_obstacle_noninteraction_2,
                                     swap_1, //swap_2,
                                     shorten_1, shorten_2};
     std::vector<MR_Parameter> lv3 ={priority_swap_1, priority_swap_2,
                                     add_agent_1, add_agent_2,
                                     delete_agent_1, delete_agent_2,
                                     window_probe_v1, window_probe_v2};
     std::vector<MR_Parameter> tested = {shorten_1,shorten_2};

     std::vector<MR_Parameter> mr_para_vect = {};
     std::move(lv1.begin(), lv1.end(), std::inserter(mr_para_vect, mr_para_vect.end()));
     std::move(lv2.begin(), lv2.end(), std::inserter(mr_para_vect, mr_para_vect.end()));
     std::move(lv3.begin(), lv3.end(), std::inserter(mr_para_vect, mr_para_vect.end()));
//     std::move(tested.begin(), tested.end(), std::inserter(mr_para_vect, mr_para_vect.end()));

     seed_setting(mr_para_vect);
     return mr_para_vect;
}
