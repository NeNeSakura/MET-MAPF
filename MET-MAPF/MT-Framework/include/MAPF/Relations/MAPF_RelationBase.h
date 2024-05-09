//
// Created by hiccup on 2020/5/7.
//

#pragma once

#include <memory>
#include <string>
#include <functional>
#include "nlohmann/json.hpp"
#include "../../API/IRelation.hpp"
#include "../../API/IOperator.hpp"
#include "MAPF_ViolationCriteria.h"

namespace TestingFramework {
    namespace MAPF {
        using std::string;
        using std::vector;

        class MAPF_MetamorphicRelationBase : public IMetamorphicRelation {

        public:
            MAPF_MetamorphicRelationBase(const MR_Parameter &param, IOperation_ptr ptr = NULL)
                    : IMetamorphicRelation(param, ptr) {}

            string TestCaseParamGenerator(string json_raw_str) override;


            //void initParameters(MR_Parameter &mr_param) override;

            /// 所有TestUnit的初始测试用例的参数生成，每个TestUnit的初始测试用例的随机种子不同
            /// \param mr_param  Relation 参数
            /// \param mt_param  Testing 参数
            /// \param origin_case_name 初始测试用例名称
            /// \return 所有Testunit中的初始测试用例的输入
            vector<string>
            originCaseParamGenerator(MR_Parameter &mr_param, MT_Parameter &mt_param,
                                     string &origin_case_name) override;

            /// 基本款的 followCaseParamGenerator, 注意，这里涉及到的MR不会使用 Origin case 的运行结果
            /// 如果需要 origin result 和 origin parameter, 则需要继承该 Base 类 并再次 override 该函数，调用Running Case 后再
            /// 运行 followParameterGeneration 获得后续的 运行输入信息
            /// 考虑到被测目标的通用性（可能无法拿到源码，可能不是c++ 程序），我们采用文件参数传入的方式解决该问题。
            /// \param origin_case_param 初始测试用例的参数
            /// \return
            vector<string>
            followCaseParamGenerator(string &origin_case_param, const string &origin_case_result) override;


            std::function<bool(const std::vector<double> &length_vect, double threshold)> violation_criteria_func;

        protected:


            vector <string> initFollowCaseParamVector(string &origin_case_param) const;
        };

    }
}
