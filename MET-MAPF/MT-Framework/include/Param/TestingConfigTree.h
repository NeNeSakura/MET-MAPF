//
// Created by hiccup on 2020/5/3.
//

#pragma once

#include <vector>
#include <string>
#include <limits>
#include <map>
#include <functional>
#include <memory>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include <boost/format/format_class.hpp>
#include <boost/pool/pool_alloc.hpp>
#include <easyloggingpp/easylogging++.h>

#include "MT_Parameter.hpp"
#include "MR_Parameter.hpp"
#include "../API/IRelation.hpp"





//不使用ptr保存，反正stl的数据都是在堆上搞的，只要一开始开了足够的内存
//就不会有copy的损耗，而且因为是整块的内存，所以也不会有内存碎片的问题。
namespace TestingFramework {

//    struct MT_Parameter;
//    struct MR_Parameter;

    class MR_TestCase;

    class MR_TestUnit;

    class MR_RelationGroup;

    class MR_ConfigTree;
    struct PoolAlloc;

    class IMetamorphicRelation;

    using std::shared_ptr;
    using MR_RelationGroup_ptr = shared_ptr<MR_RelationGroup>;
    using MR_TestUnit_ptr = shared_ptr<MR_TestUnit>;
    using MR_TestCase_ptr = shared_ptr<MR_TestCase>;
    using TestCase_PoolAlloc = boost::pool_allocator<MR_TestCase>;
    using TestUnit_PoolAlloc = boost::pool_allocator<MR_TestUnit>;
    using RelationGroup_PoolAlloc = boost::pool_allocator<MR_RelationGroup>;

    using std::string;
    using std::vector;
    using std::map;
    using boost::format;


    //TODO change the alloc?!
    struct PoolAlloc {
        TestCase_PoolAlloc testcase_pool_alloc;
        TestUnit_PoolAlloc testunit_pool_alloc;
        RelationGroup_PoolAlloc relationgroup_pool_alloc;

        void GabbageCollection();

        void ReleaseMemory();

        static PoolAlloc single_instance;
    };


    class MR_TestCase {

    public:
        string uuid_idx;
        string input_args;
        string output_args;
        shared_ptr<IMetamorphicRelation> relation_ptr;   //!< 数据源来自MR_RelationGroup
        bool is_legal_execution = false;
        MR_TestCase(const string &args, const shared_ptr<IMetamorphicRelation> &relationPtr);

//        void generateTestCase(string &input_args) {
//            this->input_args = input_args;
//            this->relation_ptr =relation_ptr;
//        };

        void generateTestCaseParam();

    };

    class MR_TestUnit {
    private:
    public:
        string uuid_idx;    //inited at @generateTestUnit
        int seed;       //inited at @generateTestUnit

        string origin_result;
        MR_TestCase_ptr origin_case;
        vector<MR_TestCase_ptr> follow_cases_list;
        shared_ptr<IMetamorphicRelation> relation_ptr;   //!< 数据源来自MR_RelationGroup
        string relation_satisfied_record;                //!< recording MR testing result.

        MR_TestUnit(size_t follow_cases_num, shared_ptr<IMetamorphicRelation> relation_ptr) {
            this->relation_ptr = relation_ptr;
            follow_cases_list.reserve(follow_cases_num);
            uuid_idx = "";
            origin_result = "";
            relation_satisfied_record = "";
            seed= std::numeric_limits<int>::min();
        }
//        //called by generateTestUnit
//        static std::function<vector<string>(MR_Parameter &mr_param, MT_Parameter &mt_param, string &origin_case_param)>
//                follow_case_param_generator;

//        //called by generateTestUnit
//        static std::function<vector<string>(shared_ptr<IMetamorphicRelation> relation_ptr,
//                                            string &origin_case_param)> follow_case_param_generator;

        void generateTestUnit(string &origin_case_param, MR_Parameter &mr_param, MT_Parameter &mt_param);
    };

    class MR_RelationGroup {
    public:
        string uuid_idx;

        MR_Parameter relation_param;
        vector<MR_TestUnit_ptr> unit_list;
        shared_ptr<IMetamorphicRelation> relation_ptr; //!< 原始数据源在这里 new 出来

//        //called by generateRelationGroup
//        //本部分实现的是对所有TestUnit 生成 origin_case_para的过程，每个TestUnit的种子不同。
//        static std::function<vector<string>(MR_Parameter &mr_param, MT_Parameter &mt_param, string &map_name)>
//                origin_case_param_generator;


        //called by generateRelationGroup
        //本部分实现的是对所有TestUnit 生成 origin_case_para的过程，每个TestUnit的种子不同。
//        static std::function<vector<string>(MR_Parameter &mr_param, MT_Parameter &mt_param, string &map_name)>
//                origin_case_param_generator;

        MR_RelationGroup(size_t iter_times) {
            unit_list.reserve(iter_times);
            uuid_idx = boost::uuids::to_string(boost::uuids::random_generator()());

        };

        void generateRelationGroup(MR_Parameter &mr_param, MT_Parameter &mt_param);
    };

    class MR_ConfigTree {
    private:
        PoolAlloc pool_alloc;

    public:


        std::vector<MR_RelationGroup_ptr> group_list;

        void Init_MR_ConfigTree(size_t MR_num);

        void GenerateTree(MT_Parameter &mt_param);

        void GabbageCollection();
    };

}



/*
//Storing shared pointers in boost pool allocators
//tl;dr It just works (tm)
//
//* If a lot of smallish objects are being allocated & freed a pool allocator
//can speed things up by specialising in allocating exactly that size of
//object.
//
//* If pointer lifetimes need to be managed with reference counting,
//std::shared_ptr can wrap an object to provide safe reference counting.
//
//* ObjPool::allocate can reduce the cost of creating a shared pointer by doing
//a single allocation for the reference counting infrastructure and data.
//
//* Boost pool allocators can be used with shared pointers by passing them into
//std::allocate_shared - see below for more details!

#include <iostream>
#include <memory>
#include <boost/pool/pool_alloc.hpp>

int main(int argc, char** argv)
{
    {
        //Allocations monitored using https://github.com/samsk/log-malloc2

        //This does a single 4 byte allocation
        int* a = new int(5);

        //This allocates 4 bytes (for the data), then 24 bytes for the
        //shared_ptr counter
        auto b = std::shared_ptr<int>(new int(5));

        //This does a single 32 byte allocation for the shared_ptr counter and
        //the data
        auto c = ObjPool::allocate<int>(5);

        //Creating a pool_allocator object doesn't actually do an allocation
        //
        //Really this acts as a reference to a set of singleton allocators
        boost::pool_allocator<int> alloc;

        //This uses malloc to get 272 bytes - 32 chunks of 8 bytes + a header
        //Allocations are 8 bytes, not 4, because when an allocation is waiting
        //to be issued that space is used to hold a pointer (and I'm doing this
        //on a 64 bit machine). The pointers form a linked list of all the
        //free blocks waiting to be issued
        int* d = alloc.allocate(1);

        //This doesn't result in any additional calls to malloc
        for(size_t i=0;i<31;i++)
            d = alloc.allocate(1);

        //This causes the pool allocator to ask malloc for an extra 528 bytes.
        //That's enough to issue another 64 allocations. The pool allocator
        //will keep asking for twice as much memory from malloc until it is
        //able to satisfy all of the requests from its internal store
        d = alloc.allocate(1);

        //std::allocate_shared does a single allocation for data & reference
        //counting information, just like make_shared. The difference is that
        //allocate_shared can use an allocator other than malloc.
        //
        //A pool allocator specialised to dole out 4 byte chunks of memory
        //will not be able to satisfy a request from allocate_shared for 32
        //bytes.
        //
        //When allocate_shared uses the pool it calls rebind_traits to get a
        //*new* allocator that issues 32 bytes at a time. This new allocator is
        //registered as a singleton, so it is reused.
        //
        //This call causes a pool_allocator to be crated that gets 1040 bytes
        //from malloc - enough for 32 shared_ptrs to integers including the
        //integer.
        auto p = std::allocate_shared<int, boost::pool_allocator<int>>(alloc, 5);

        //This doesn't cause any more memory to be retrieved from malloc
        std::shared_ptr<int> ptrs[31];
        for(size_t i=0;i<31;i++)
        {
            ptrs[i] = std::allocate_shared<int, boost::pool_allocator<int>>(alloc, 5);
        }
    }
}
 */
