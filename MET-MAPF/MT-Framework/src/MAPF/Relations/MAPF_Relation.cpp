//
// Created by hiccup on 2020/5/5.
//

#include "../../../include/MAPF/Relations/MAPF_Relation.h"
#include "../../../include/MAPF/Relations/MAPF_CoordinateRelations.h"
#include "../../../include/MAPF/Relations/MAPF_MissionRelations.h"
#include "../../../include/MAPF/Relations/MAPF_InteractiveRelations.h"
#include "../../../include/MAPF/Relations/MAPF_ObstacleRelations.h"
#include "../../../include/MAPF/Relations/MAPF_RandomTestingRelations.hpp"

namespace TestingFramework {
    namespace MAPF {

        MT_Relation_ptr MAPF_RelationGenerator(MR_Parameter &mr_param) {
            if(mr_param.reference_class_name == "Rotation")
            {
                auto ptr = ObjPool::allocate<Rotation>(mr_param);
                return ptr;
            }
            else if(mr_param.reference_class_name == "Mirror")
            {
                auto ptr = ObjPool::allocate<Mirror>(mr_param);
                return ptr;
            }
            else if(mr_param.reference_class_name == "Exchange")
            {
                auto ptr = ObjPool::allocate<Exchange>(mr_param);
                return ptr;
            }
            else if(mr_param.reference_class_name == "ExchangeRandom")
            {
                auto ptr = ObjPool::allocate<ExchangeRandom>(mr_param);
                return ptr;
            }
            else if(mr_param.reference_class_name == "PriorityExchange")
            {
                return ObjPool::allocate<PriorityExchange>(mr_param);
            }
//            else if(mr_param.reference_class_name == "PriorityExchangeRandom")
//            {
//                return ObjPool::allocate<PriorityExchangeRandom>(mr_param);
//            }
            else if(mr_param.reference_class_name == "DeleteAgent")
            {
                return ObjPool::allocate<DeleteAgent>(mr_param);
            }
            else if(mr_param.reference_class_name == "DecreaseObstacle")
            {
                return ObjPool::allocate<DecreaseObstacle>(mr_param);
            }
            else if(mr_param.reference_class_name == "IncreaseObstacle"||
                    mr_param.reference_class_name == "IncreaseObstacleRandom"||
                    mr_param.reference_class_name == "IncreaseObstacleNoninteraction")
            {
                return ObjPool::allocate<IncreaseObstacle>(mr_param);
            }
            else if(mr_param.reference_class_name == "RandomTesting")
            {
                return ObjPool::allocate<RandomTesting>(mr_param);
            }
            else if(mr_param.reference_class_name == "AddRandomPriorityProbe")
            {
                return ObjPool::allocate<AddRandomPriorityProbe>(mr_param);
            }
            else if(mr_param.reference_class_name == "AddWindowedPriorityProbe")
            {
                return ObjPool::allocate<AddWindowedPriorityProbe>(mr_param);
            }
            else if(mr_param.reference_class_name == "ShrinkStartPos")
            {
                return ObjPool::allocate<ShrinkStartPos>(mr_param);
            }
            else if(mr_param.reference_class_name == "AddAgent")
            {
                return ObjPool::allocate<AddAgent>(mr_param);
            }
            else if(mr_param.reference_class_name == "Reshape")
            {
                json j = json::parse(mr_param.MR_para_str);
                if( j.contains("angle"))
                    return ObjPool::allocate<Rotation>(mr_param);
                else if(j.contains("direction"))
                    return ObjPool::allocate<Mirror>(mr_param);
                else{
                    LOG(ERROR)<< "Illegal mr_param reference_class_name";
                    return nullptr ;
                }
            }
            else
            {
                LOG(ERROR)<< "Illegal mr_param reference_class_name";
                return nullptr ;
            }
        }
    }

}


