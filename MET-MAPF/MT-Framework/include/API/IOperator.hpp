#pragma once
#include <boost/geometry.hpp>

namespace TestingFramework {

    using std::string;

    class IOperator {
    using IOperation_ptr = std::shared_ptr<IOperator>;

//    protected:
//        const double TOLERANCE = 1e-10;
    public:
        string operator_name;
        string operator_param;
        IOperation_ptr follow_operation_ptr = NULL;
        virtual void RelationOperation(string &testcase_param_json, const string &origincase_result = "") = 0 ;
    };


}
