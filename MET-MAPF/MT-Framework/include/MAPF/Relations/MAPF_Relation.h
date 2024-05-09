//
// Created by hiccup on 2020/5/5.
//

#pragma once


#include "../../API/IRelation.hpp"

namespace TestingFramework {
    namespace MAPF {

        using MT_Relation_ptr = shared_ptr<IMetamorphicRelation>;

        //including
        //class Mirror
        //class Rotate
        //class Exchange(ramdom / all)
        //class PriorityChange (fixed /random)
        //class RemoveAgent(random priortiy, MaxPriority);
        //class IncreaseObstacle
        //class DecreaseObstacle

        MT_Relation_ptr MAPF_RelationGenerator(MR_Parameter &mr_param);
    }
}

