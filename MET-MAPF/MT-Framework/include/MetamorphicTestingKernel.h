//
// Created by hiccup on 2020/5/2.
//

#pragma once

#include <vector>
#include <string>
#include <list>
#include <functional>

#include "Param/TestingConfigTree.h"
#include "Param/MT_Parameter.hpp"
#include "Param/MR_Parameter.hpp"
#include "MAPF/Execution/MAPF_ExecutionBase.h"
#include "MAPF/Execution/MAPF_Execution.h"
#include "MetamorphicTestingKernel.h"
#include "ART-Framework/Param/KernelParam.hpp"





//MAPF_Execution 为临时挂上的啊。
//最终是要写个壳子吧TestingFrame初始化的。

namespace TestingFramework {

    class IMetamorphicRelation;

    class MT_Parameter;

    class MR_ConfigTree;

    struct PoolAlloc;


    ///using part
    using std::vector;
    using std::string;
    using std::shared_ptr;
    using MT_Relation_ptr = shared_ptr<IMetamorphicRelation>;

    class MetamorphicTestingKernel {
    public:
        string MT_param_str;

        MetamorphicTestingKernel(string const &para_filename);

        void execution();

        void collectResultInfo();

        void analyzeResultInfo();

        void initTesting(std::function<MT_Relation_ptr(MR_Parameter &)> relation_generator);

        MR_ConfigTree getConfigTree();

        static std::function<MT_Relation_ptr(MR_Parameter &)> relationGeneratorFunc;
//        static std::function<void()> testingCodeExecutionFunc;
//        static std::function<string()> originTestCaseResultReaderFunc;
        void executionART(ARTTestingFramework::ARTKernelParam ART_kernel_param);

    private:
        ///
        /// \param para_filename: MT_parameter file address (*.json)
        /// \return metamorphic testing parameter struct
        void readPara(const string &para_filename);

        ///
        /// \param jf: Testing Stream parameter
        /// \return problem flag
        bool checkParam(MT_Parameter &param);


//        bool executeFollowUpTestCase(MR_TestUnit_ptr unit_ptr);
//        bool lazyGenerateFollowUpTestCase(MR_TestUnit_ptr unit_ptr);
//        bool executeOriginTestCase(MR_TestUnit_ptr unit_ptr);



    private:
        MT_Parameter _mt_param;

        MR_ConfigTree _config_tree;

        //PoolAlloc pool_alloc;

        shared_ptr<MAPF_ExecutionBase>  _execute_ptr;


        void SaveMTparam() const;

    };


}


