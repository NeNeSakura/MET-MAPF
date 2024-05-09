//
// Created by hiccup on 2021/2/3.
//

#pragma once


#include <vector>
#include <algorithm>
#include <stdexcept>

namespace TestingFramework {
    namespace MAPF {
        using std::vector;
        using std::all_of;
        struct ViolationCriteriaData{

        };

        class ViolationCriteria : ViolationCriteriaData {
        public:
            enum ThresholdTypeEnum {
                AbsoluteValue,
                RatioValue,
            };

        public:
            static double Threshold;
            static ThresholdTypeEnum ThresholdType;

        public:
            template<typename T>
            static bool Less(const vector<T> &lengths, const double threshold = Threshold) {
                if (lengths.size() < 2)
                    throw std::length_error("size for input less than 2");

                if (ThresholdType == RatioValue) {

                    if (threshold >= 1 || threshold <= 0)
                        throw std::out_of_range("threshold should be in (0,1)");

                    return all_of(lengths.begin() + 1, lengths.end(),
                                  [&](auto i) {
                                      return i <= lengths[0] * (1 + threshold);
                                  });
                } else {
                    return all_of(lengths.begin() + 1, lengths.end(),
                                  [&](auto i) {
                                      return i <= lengths[0] + threshold;
                                  });
                }

            }

            template<typename T>
            static bool Greater(const vector<T> &lengths, const double threshold = Threshold) {
                if (lengths.size() < 2)
                    throw std::length_error("size for input less than 2");


                if (ThresholdType == RatioValue) {
                    if (threshold >= 1 || threshold <= 0)
                        throw std::out_of_range("threshold should be in (0,1)");

                    return all_of(lengths.begin() + 1, lengths.end(),
                                  [&](auto i) {
                                      return i >= lengths[0] * (1 - threshold);
                                  });
                } else {
                    return all_of(lengths.begin() + 1, lengths.end(),
                                  [&](auto i) {
                                      return i >= lengths[0] - threshold;
                                  });
                }
            }

            template<typename T>
            static bool Equal(const vector<T> &lengths, const double threshold = Threshold) {
                if (lengths.size() < 2)
                    throw std::length_error("size for input less than 2");

                if (ThresholdType == RatioValue) {

                    if (threshold >= 1 || threshold <= 0)
                        throw std::out_of_range("threshold should be in (0,1)");

                    return std::all_of(lengths.begin() + 1, lengths.end(),
                                       [&](auto i) {
                                           return i > lengths[0] * (1 - threshold) &&
                                                  i < lengths[0] * (1 + threshold);
                                       });
                } else {
                    return std::all_of(lengths.begin() + 1, lengths.end(),
                                       [&](auto i) {
                                           return i > lengths[0] - threshold &&
                                                  i < lengths[0] + threshold;
                                       });
                }
            }

        };


    }
}