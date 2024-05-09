//
// Created by hiccup on 2021/5/31.
//

#pragma once

#include<string>
#include <boost/thread.hpp>
#include "concurrentqueue/blockingconcurrentqueue.h"
#include <eigen3/Eigen/Core>


#include "Distance.hpp"
#include "../Datas/MTData.h"

namespace ARTTestingFramework {
    using std::string;
//    using Point2 = TestingFramework::MAPF::Point2D<int>;
    using TestingFramework::MAPF::MAPF_TaskItem;


    class IMeasure {


    protected:
        IDistance<FPoint2> distance_instance;

    public:


        string name;
        string description;
        std::function<double(const FPoint2 &, const FPoint2 &)> distance_func; //set the norm of the distance_func

        virtual double
        GetMeasureValue(const std::vector<MeasureItemData> &item_data_list, const MTItemData &item_data) {
//            tmp.clear();
//            tmp.reserve(item_data_list.size());
////            tmp.resize(item_data_list.size());
//            for(const auto &item : item_data_list)
//                tmp.push_back(item.MT_itemData);
//            return this->GetMeasureValue(tmp,item_data);
            return 0;
        };

        virtual double GetMeasureValue(const std::vector<MTItemData> &item_data_list, const MTItemData &item_data) {
            //            ScenarioJsonReader()
            return 0;
        };
    };

    class TaskMeasure : public IMeasure {

    protected:
        using matrix_type = Eigen::MatrixXf;
        using matrix_type_ptr = shared_ptr<matrix_type>;
        std::vector<matrix_type_ptr> _matrix_idd_vect; // refactor for speed up(used by GetMeasureValue).
        std::vector<matrix_type_ptr> _matrix_ddd_vect; // refactor for speed up(used by GetMeasureValue).

    private:
        moodycamel::ConcurrentQueue<std::array<matrix_type_ptr,2>> queue;
        int producer_thread_count;
        int consumer_thread_count;
        boost::mutex m;
        boost::thread_group producer_threads, consumer_threads;
        const int threshold_size = 100;




    public:
        TaskMeasure() {
            name = "Task";
            description = "start + target";
            distance_instance = ExpEuclidDistance<FPoint2>();
            distance_func = [&](const FPoint2 &lhs, const FPoint2 &rhs) {
                return distance_instance.GetDistance(lhs, rhs);
            };

            producer_thread_count = 4;
            consumer_thread_count = 4;

        }


        virtual double
        GetMeasureValue(const std::vector<MTItemData> &item_data_list, const MTItemData &item_data) override {
//            return GetMeasureValueSimple(item_data_list, item_data);

            if (item_data_list.size() == 0)
                return 0;

            if (item_data_list.size() < threshold_size)
                return GetMeasureValueSimple(item_data_list, item_data);

            std::atomic<int> doneProducers(0);
            std::atomic<int> doneConsumers(0);

            //init values!
            std::vector<float> values;
            values.reserve(_matrix_idd_vect.size());

            //producer part
            for (int i = 0; i < producer_thread_count; i++) {

                producer_threads.create_thread([&, i]() { //i should be the copy , not the references!
                    int idx = i;
                    while (idx < (int) item_data_list.size()) {

                        if (item_data_list.size() != 0) {
                            if (item_data_list[0].scenario.task_info.size() != item_data.scenario.task_info.size())
                                throw std::range_error("lhs.size not equal to rhs.size in Func GetMeasureValue of TaskMeasure ");
                        }

                        auto idd_ptr = GetDDMatrix(item_data, 0);
                        auto ddd_ptr = GetDDMatrix(item_data, 1);
                        queue.enqueue(std::array<matrix_type_ptr,2>{idd_ptr,ddd_ptr});
                        idx += producer_thread_count;
                    }


                    doneProducers.fetch_add(1, std::memory_order_release);
                });
            }
            //consumer part

            for (int i = 0; i < consumer_thread_count; ++i) {
                consumer_threads.create_thread([&]() {
                    std::array<matrix_type_ptr,2> rhs;
                    bool itemsLeft;
                    do {
                        // It's important to fence (if the producers have finished) *before* dequeueing
                        itemsLeft = doneProducers.load(std::memory_order_acquire) != producer_thread_count;
                        while (queue.try_dequeue(rhs)) {
                            itemsLeft = true;


                            auto len_s = GetDistance(item_data.scenario.map_info.width, item_data.scenario.map_info.height,
                                                     rhs[0], _matrix_idd_vect[i]);
                            auto len_g = GetDistance(item_data.scenario.map_info.width, item_data.scenario.map_info.height,
                                                     rhs[1], _matrix_ddd_vect[i]);

                            m.lock();
                            values.push_back(std::max(len_s, len_g));
                            m.unlock();
                        }
                    } while (itemsLeft ||
                             doneConsumers.fetch_add(1, std::memory_order_acq_rel) + 1 == consumer_thread_count);
                    // The condition above is a bit tricky, but it's necessary to ensure that the
                    // last consumer sees the memory effects of all the other consumers before it
                    // calls try_dequeue for the last time
                });
            }


            producer_threads.join_all();
            consumer_threads.join_all();

            auto iter = std::min_element(values.begin(), values.end());
            return *iter;
        };




        matrix_type_ptr
        GetDDMatrix(const MTItemData &item_data,int dd_type) {
            const TestingFramework::MAPF::MAPF_Task &task_info = item_data.scenario.task_info;
            const TestingFramework::MAPF::MAPF_Map &map_info = item_data.scenario.map_info;

            matrix_type_ptr result = std::make_shared<matrix_type>(
                    map_info.width,
                    map_info.height);
            int step = 2;

            if (dd_type == 0) {
                for (const auto &_info : task_info) {
                    for (int i = -step; i <= step; ++i)
                        for (int j = -step; j <= step; ++j) {
                            if (i + _info.start.x >= 0 && i + _info.start.x < map_info.width &&
                                j + _info.start.y >= 0 && j + _info.start.y < map_info.height)
                                (*result).coeffRef(i + _info.start.x,j + _info.start.y)
                                        += distance_func(_info.start, FPoint2(i, j) + _info.start);
                        }
                }
            } else if (dd_type == 1) {
                for (const auto &_info : task_info) {
                    for (int i = -step; i <= step; ++i)
                        for (int j = -step; j <= step; ++j) {
                            if (i + _info.goal.x >= 0 && i + _info.goal.x < map_info.width &&
                                j + _info.goal.y >= 0 && j + _info.goal.y < map_info.height)
                                (*result).coeffRef(i + _info.goal.x,j + _info.goal.y)
                                        += distance_func(_info.goal, FPoint2(i, j) + _info.goal);
                        }
                }
            }
            return result;

        }

        double GetMeasureValueSimple(const std::vector<MTItemData> &item_data_list, const MTItemData &item_data) {
            if (item_data_list.size() == 0)
                return 0;

            //init values!
            std::vector<float> values;
            values.reserve(item_data_list.size());
            const auto &lhs = item_data.scenario.task_info;
            std::atomic<int> promisedElementsRemaining(item_data_list.size());

            //producer consumer part
//            producer_threads.create_thread()

            if (item_data_list.size() != 0) {
                if (item_data_list[0].scenario.task_info.size() != item_data.scenario.task_info.size())
                    throw std::range_error("lhs.size not equal to rhs.size in Func GetMeasureValue of TaskMeasure ");
            }

            auto idd_ptr = GetDDMatrix(item_data, 0);
            auto ddd_ptr = GetDDMatrix(item_data, 1);


            for (int i = 0; i< _matrix_idd_vect.size(); ++i) {
                auto len_s = GetDistance(item_data.scenario.map_info.width, item_data.scenario.map_info.height,
                                           idd_ptr, _matrix_idd_vect[i]);
                auto len_g = GetDistance(item_data.scenario.map_info.width, item_data.scenario.map_info.height,
                                    ddd_ptr, _matrix_ddd_vect[i]);
                values.push_back(std::max(len_s, len_g));
            }

            auto iter = std::min_element(values.begin(), values.end());
            return *iter;
        };

        float GetDistance(int width, int height, matrix_type_ptr lhs, matrix_type_ptr rhs) {
            return (*lhs- *rhs ).squaredNorm();
        }
    };

    class MapMeasure : public IMeasure {
    public:
        MapMeasure() {
            name = "Map";
            description = "Map diff";
            distance_instance = ExpEuclidDistance<FPoint2>();
            distance_func = [&](const FPoint2 &lhs, const FPoint2 &rhs) {
                return distance_instance.GetDistance(lhs, rhs);
            };
//            distance_func = EuclidDistance<FPoint2>; // not used?!
        }

        virtual double
        GetMeasureValue(const std::vector<MTItemData> &item_data_list, const MTItemData &item_data) override {
            if (item_data_list.size() == 0)
                return 0;

            std::vector<double> values;
            const auto &lhs = item_data.scenario.map_info;
            for (auto iter = item_data_list.begin(); iter != item_data_list.end(); iter++) {
                const auto &rhs = iter->scenario.map_info;
                if (lhs.width != rhs.width || lhs.height != rhs.height)
                    throw std::range_error("lhs.size not equal to rhs.size in Func GetMeasureValue of MapMeasure ");

                double len = 0;
                for (auto i = 0; i < lhs.map_data.size(); ++i) {
                    if (lhs.map_data[i] == rhs.map_data[i])
                        len += 1;
                }

                values.push_back(len);

            }
            auto iter = std::min_element(values.begin(), values.end());
            return *iter;
        };
    };
}