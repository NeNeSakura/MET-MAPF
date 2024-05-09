//
// Created by hiccup on 2020/5/4.
//

#pragma once

#include <string>
#include <boost/geometry.hpp>

#include "../Param/TestingConfigTree.h"
#include "../RandomHelper.h"
#include "../Param/MT_Parameter.hpp"
#include "IOperator.hpp"

namespace TestingFramework {

    class MR_TestCase;
    class MR_TestUnit;

    class MT_Parameter;

    class MR_Parameter;

    using std::string;
    using std::shared_ptr;
    using MT_Parameter_ptr = shared_ptr<MT_Parameter>;
    using MR_Parameter_ptr = shared_ptr<MR_Parameter>;
    using MR_TestUnit_ptr = shared_ptr<MR_TestUnit>;
    using MR_TestCase_ptr = shared_ptr<MR_TestCase>;
    using IOperation_ptr = std::shared_ptr<IOperator>;


    /// <summary>
    /// MT 接口定义
    /// </summary>
    class IMetamorphicRelation {
    protected:

        MR_Parameter MR_param;
        //MT_Parameter MT_param;

    public:

        IMetamorphicRelation(const MR_Parameter &param, IOperation_ptr ptr = NULL)
                : instance_name(param.name), MR_param(param) ,operator_ptr(ptr) {};
        string instance_name;
        IOperation_ptr operator_ptr = NULL;

        //called by generateTestCaseParam
        virtual string TestCaseParamGenerator(string json_raw_str) = 0;

        //called by generateRelationGroup
        //本部分实现的是对所有TestUnit 生成 origin_case_para的过程，每个TestUnit的种子不同。
        virtual vector<string>
        originCaseParamGenerator(MR_Parameter &mr_param, MT_Parameter &mt_param, string &origin_case_name) = 0;

        //called by generateTestUnit
        virtual vector <string>
        followCaseParamGenerator(string &origin_case_param, const string &origin_case_result) = 0;

        //virtual void initParameters(MR_Parameter &mr_param) = 0;

        virtual bool isRelationSatisfied(const string &origin_result, const vector <string> &follow_results) = 0;

        //virtual bool isRelationSatisfied(string &origin_case, vector<string> &follow_cases) = 0;

    protected:
//        virtual string resultDeserializing(const MR_TestCase_ptr testcase_ptr) = 0;

    };


}




