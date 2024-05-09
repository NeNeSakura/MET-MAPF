//
// Created by hiccup on 2021/5/31.
//

#pragma once

#include <cmath>
#include <unordered_map>
#include <string>

namespace ARTTestingFramework {
    template<class T>
    class IDistance{
    public:
        virtual double GetDistance(const T &lhs,const T &rhs) {
            return 0;
        };
        virtual double operator()(const T &lhs,const T &rhs) {
            return GetDistance(lhs,rhs);
        };
    };

    template<class T>
    class ExpEuclidDistance: public IDistance<T>{
    private:
        std::unordered_map<std::string, double> value_cache;
        bool is_cached = true;
        std::mutex mutex;

    public:
        ExpEuclidDistance(bool cached = true)
        {
            is_cached = cached;
        }
//        EuclidDistance() {
//
//        }
        virtual double GetDistance(const T &lhs,const T &rhs) override{
            auto diff = rhs-lhs;
            if(is_cached)
            {
                std::string k = std::to_string(int(diff.x)) + "," + std::to_string(int(diff.y));
                if(value_cache.count(k) != 0)
                {
                    return value_cache[k];
                }
                else
                {
                    mutex.lock();
                    if(value_cache.count(k) == 0)
                    {
                        auto val = std::exp(std::sqrt(diff.x *diff.x + diff.y*diff.y)) ;
                        value_cache[k] = val;
                        mutex.unlock();
                        return val;
                    }
                    else
                    {
                        mutex.unlock();
                        return value_cache[k];
                    }
                }
            }

            return std::exp(std::sqrt(diff.x *diff.x + diff.y*diff.y));
        };

//        virtual double GetDistance(const T &lhs,const T &rhs) override{
//            auto diff = rhs-lhs;
//            if(is_cached)
//            {
//                std::string k = std::to_string(int(diff.x)) + "," + std::to_string(int(diff.y));
//                if(value_cache.count(k) != 0)
//                {
//                    return value_cache[k];
//                }
//                else
//                {
//                    auto val = std::exp(std::sqrt(diff.x *diff.x + diff.y*diff.y)) ;
//                    value_cache[k] = val;
//                    return val;
//                }
//            }
//
//            return std::exp(std::sqrt(diff.x *diff.x + diff.y*diff.y));
//        };


    };

    template<class T>
    class ManhattanDistance: public IDistance<T>{
    public:
        virtual double GetDistance(const T &lhs,const T &rhs) override{
            auto diff = rhs-lhs;
            return std::abs(diff.x) + std::abs(diff.y);
        };
    };

//    template<class T>
//    double EuclidDistance(const T &lhs, const T &rhs) {
//        auto diff = rhs - lhs;
//        return std::sqrt(diff.x * diff.x + diff.y * diff.y);
//    };
//
//    template<class T>
//    double ManhattanDistance(const T &lhs, const T &rhs) {
//        auto diff = rhs - lhs;
//        return std::abs(diff.x) + std::abs(diff.y);
//    };
}