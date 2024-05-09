//
// Created by hiccup on 2020/5/2.
//
#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include <boost/format.hpp>
#include <nlohmann/json.hpp>

#include "MR_Parameter.hpp"


namespace TestingFramework {
    using std::exception;
    using std::cerr;
    using std::endl;
    using nlohmann::json;

    /// Metamorphic Testing Parameter
    struct MT_Parameter {

        string description;

        // MT parameter
        size_t repeat_num;
        vector<MR_Parameter> MR_list;
        vector<string> cases_list;  //!< origin cases_list
        string MT_hyper_param;      //!< MT hyper parameters by dumped json string;

        //IO info
        string case_path_root;      //!< map root
        string MR_path_root;        //!< metamorphic relation .so files root (TODO)
        string data_path_root;      //!< experiment result generation root
        string tmp_file_path_root;  //!< temp file generate root, considering the fact that frequent file
                                    //!< reading and writing will affect the life of the hard disk


    };

    inline void to_json(json &j, const MT_Parameter &p) {
        j = json{{"description",        p.description},
                 {"repeat_num",         p.repeat_num},
                 {"MR_list",            p.MR_list},
                 {"cases_list",         p.cases_list},
                 {"MT_hyper_param",     p.MT_hyper_param},
                 {"case_path_root",     p.case_path_root},
                 {"MR_path_root",       p.MR_path_root},
                 {"data_path_root",     p.data_path_root},
                 {"tmp_file_path_root", p.tmp_file_path_root}};
    }

    inline void from_json(const json &j, MT_Parameter &p) {
        j.at("description").get_to(p.description);

        j.at("repeat_num").get_to(p.repeat_num);
        j.at("MR_list").get_to(p.MR_list);
        j.at("cases_list").get_to(p.cases_list);
        j.at("MT_hyper_param").get_to(p.MT_hyper_param);

        j.at("case_path_root").get_to(p.case_path_root);
        j.at("MR_path_root").get_to(p.MR_path_root);
        j.at("data_path_root").get_to(p.data_path_root);
        j.at("tmp_file_path_root").get_to(p.tmp_file_path_root);
    }

}

