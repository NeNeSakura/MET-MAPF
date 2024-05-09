//
// Created by hiccup on 2020/5/6.
//

#pragma once

#include <vector>
#include <random>
#include <string>

#include "MAPF_Point2D.hpp"
#include "../Param/MR_Parameter.hpp"
#include "../Param/MT_Parameter.hpp"
#include "../API/IOperator.hpp"
#include "../API/IRelation.hpp"
#include "Operator/MAPF_Operator.h"
#include "Relations/MAPF_Relation.h"
#include "Relations/MAPF_RelationHelper.h"


namespace TestingFramework {
    namespace MAPF {
        using std::vector;
        using std::string;

        class MAPF_TestDemo {

            vector<FPoint2> starts;
            vector<FPoint2> goals;

        public:

            void MapReader(std::string &filename);

            vector<std::pair<FPoint2, FPoint2>> getRandomStartGoal(unsigned num,
                                                                   std::mt19937 &MT);
            std::string getRandomStartGoal_JsonStr(unsigned int num, std::mt19937 &MT);

            string RawMapReader(string &filename);

            void TestCaseParaGenerateDemo();

            json MT_Para_Demo();

            json MT_Para_Demo2();
        };


    }
}


