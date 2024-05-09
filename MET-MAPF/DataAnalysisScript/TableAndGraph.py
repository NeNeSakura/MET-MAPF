import json
from dataclasses import dataclass

import numpy as np
import pandas as pd
from typing import List, Tuple, Optional
import os.path
from numpy.core.defchararray import rindex
from collections import defaultdict

import MyPlot

specific_MR_name = True
# specific_MR_name = False

if specific_MR_name:
    # Type A: define the Order:
    # print_mr_name_list = list(ReadFile.convert_name_dict.values())
    print_mr_name_list = ["flip", "rmObs", "addObs", "swap", "addIrrObs", "shorten", "addAgent", "rmAgent", "priorex",
                          "probe"]
    print_layer_name_list = ["EL", "SAL", "MAL"]
else:
    # Type B: no order :
    print_mr_name_list = []  # update in readfile
    print_layer_name_list = []  # update in readfile


def data_collation(data_dict: dict) -> dict:
    data = {}
    for key in data_dict.keys():
        for value in data_dict[key]:
            j = json.loads(value["mt_para"])
            data[(key, j["ratio"])] = {
                "legal_cases": value["legal_cases"],
                "legal_groups": value["legal_units"],
                'legal_violations': value['legal_violations'],
                "MR_relation_finding": value["MR_relation_finding"],
                "legal_group_percent": value["legal_unit_percent"],
                "legal_case_percent": value["legal_case_percent"],
                'legal_violation_percent': value['legal_violation_percent'],
                "MR_relation_finding_percent": value["MR_relation_finding_percent"],
                "layer": value["layer"]}

    return data


def create_table_file(filename: str, data_dict: dict) -> List[pd.DataFrame]:
    data = data_collation(data_dict)
    tmp = []
    for key in data.keys():
        mr_name, agent_num = key
        tmp.append([mr_name, agent_num,
                    data[key]["legal_cases"], data[key]["legal_case_percent"],
                    data[key]["legal_groups"], data[key]["legal_group_percent"],
                    data[key]['legal_violations'], data[key]['legal_violation_percent'], ])

        tmp.sort(key=lambda item: item[0] + str(item[1]))
    df1 = pd.DataFrame(data=tmp, columns=["mr_name",
                                          "agent_num",
                                          "legal_cases(T/F)",
                                          "legal_case_percent",
                                          "legal_groups(T/F)",
                                          "legal_group_percent",
                                          'legal_violations(T/F)',
                                          'legal_violation_percent'])

    # print(df1)
    print(filename)
    if filename != "":
        df1.to_csv(filename)


def create_table_file_intense(filename: str, data_dict: dict) -> pd.DataFrame:
    data = data_collation(data_dict)
    # data = data_dict
    tmp = []
    for key in data.keys():
        mr_name, agent_num = key

        # case_group_info = data[key]["legal_cases"] + "(" + str(data[key]["legal_case_percent"])  + ")" + "|" + \
        #                   data[key]["legal_groups"] + "(" + data[key]["legal_group_percent"] + ")"
        case_info = "{}/{}:({:.2%})" \
            .format(data[key]["legal_cases"]["t"],
                    data[key]["legal_cases"]["f"],
                    data[key]["legal_case_percent"])

        group_info = "{}/{}:({:.2%})" \
            .format(data[key]["legal_groups"]["t"],
                    data[key]["legal_groups"]["f"],
                    data[key]["legal_group_percent"])

        MR_relation_finding = "{}/{}:({:.2%})" \
            .format(data[key]["MR_relation_finding"]["t"],
                    data[key]["MR_relation_finding"]["f"],
                    data[key]["MR_relation_finding_percent"])

        MR_violation_finding = "{}/{}:({:.2%})" \
            .format(data[key]["legal_violations"]["t"],
                    data[key]["legal_violations"]["f"],
                    data[key]["legal_violation_percent"])

        tmp.append([data[key]["layer"], mr_name, agent_num,
                    case_info, group_info, MR_relation_finding, MR_violation_finding])
    # tmp.sort(key=lambda item: item[0] + str(item[1]) + str(item[2]))
    # if zip crush , check the print_mr_name_list is correct, maybe you shuold change the type.

    tmp_sort_idx = [print_mr_name_list.index(item[1]) for item in tmp]
    zipped_tmp = sorted(zip(tmp_sort_idx, tmp))
    _, tmp = zip(*zipped_tmp)
    df1 = pd.DataFrame(data=tmp, columns=["layer",
                                          "mr_name",
                                          "agent_num",
                                          "test cases(T/F):success",
                                          "test groups(T/F):success",
                                          "MR_relation_finding(T/F):success",
                                          'MR_violation_finding(T/F):success'])

    # tmp2 = []
    # for key in data.keys():
    #     mr_name, agent_num = key
    #     case_info = 1 - data[key]["legal_case_percent"]
    #
    #     group_info = 1 - data[key]["legal_group_percent"]
    #
    #     MR_relation_finding = data[key]["MR_relation_finding_percent"]
    #
    #     tmp2.append([data[key]["layer"], mr_name, agent_num,
    #                  case_info, group_info, MR_relation_finding])
    #
    # tmp2_sort_idx = [ print_name_list.index(item[1]) for item in tmp2]
    # zipped_tmp2 = sorted(zip(tmp2_sort_idx,tmp2))
    # _, tmp2 = zip(*zipped_tmp2)
    #
    # df2 = pd.DataFrame(data=tmp2, columns=["layer",
    #                                       "mr_name",
    #                                       "agent_num",
    #                                       "test cases:failure rate",
    #                                       "test groups:failure rate",
    #                                       "MR_relation_finding"])

    # print(df1)
    print(filename)
    if filename != "":
        df1.to_csv(filename)
        # df2.to_csv(filename + "short")
        # df2.to_excel(filename + "short")


def create_layer_table_file_intense(filename: str, data_dict: dict, basepath: str = '') -> pd.DataFrame:
    df_list = GetOriginDataFrame(data_dict)
    # name_list = filename
    if len(filename) != len(df_list):
        print("len(filename) != len(df_list), print the default name")
        filename = [(basepath + "/") + "output_layer" + str(s + 1) + ".csv" for s in range(len(df_list))]
    print(filename)
    for idx, df in enumerate(df_list):
        if idx == 8 or idx == 10:
            df.to_csv(filename[idx])
            df.to_excel(filename[idx][0:-3] + "xlsx")

    # df10_MR_per_scene_failure_count = df_list[9]

    # df_MR_per_scene_failure_rate = df_list[10]
    # scenes = df_MR_per_scene_failure_rate.index.get_level_values(0).unique()
    #
    # new_folder = os.path.join(os.path.split(filename[idx])[0], "scene_MR_statistics")
    # if not os.path.exists(new_folder):
    #     os.makedirs(new_folder)
    #
    # for scene in scenes:
    #     split = df_MR_per_scene_failure_rate.xs(scene)
    #     agent_list = sorted(list(split.columns.get_level_values(1).unique()))
    #
    #     MyPlot.draw_mutiple_bar_plot_for_mr_scenes(split,
    #                                                new_folder + "/" + scene + ".png",
    #                                                print_mr_name_list,
    #                                                agent_list)
    # # =================================================
    #
    # # df_Layer_per_scene_failure_count = df_list[11]
    # df_Layer_per_scene_failure_rate = df_list[12]
    # scenes = df_Layer_per_scene_failure_rate.index.get_level_values(0).unique()
    # new_folder = os.path.join(os.path.split(filename[idx])[0], "scene_layer_statistics")
    # if not os.path.exists(new_folder):
    #     os.makedirs(new_folder)
    #
    # for scene in scenes:
    #     split = df_Layer_per_scene_failure_rate.xs(scene)
    #     agent_list = sorted(list(split.columns.get_level_values(1).unique()))
    #
    #     MyPlot.draw_mutiple_bar_plot_for_layer_scenes(split,
    #                                                   new_folder + "/" + scene + ".png",
    #                                                   print_layer_name_list,
    #                                                   agent_list)

    return df_list


def GetOriginDataFrame(data_dict: dict) -> pd.DataFrame:
    # pd.__version__

    tmp = []
    for key in data_dict.keys():
        for value in data_dict[key]:
            j = json.loads(value["mt_para"])
            item = value
            item["name"] = key
            item["agent"] = j["ratio"]
            item["pos_case"] = value["legal_cases"]["t"]
            item["neg_case"] = value["legal_cases"]["f"]
            item["pos_unit"] = value["legal_units"]["t"]
            item["neg_unit"] = value["legal_units"]["f"]
            item["pos_MR_finding"] = value["MR_relation_finding"]["t"]
            item["neg_MR_finding"] = value["MR_relation_finding"]["f"]
            item['pos_MR_violation'] = value['legal_violations']['t'][0] \
                if isinstance(value['legal_violations']['t'], tuple) else value['legal_violations']['t']
            item['neg_MR_violation'] = value['legal_violations']['f'][0] \
                if isinstance(value['legal_violations']['f'], tuple) else value['legal_violations']['f']

            item["name_merge"] = name_para_merge_converter(item['name'])

            # merged_info = value['MR_violation_failed_scene_info'] \
            #               + value['MR_transmission_failed_scene_info'] \
            #               + value['MR_VT_failed_scene_info']

            # merged_info = value['MR_violation_failed_scene_info'] \
            #               + value['MR_transmission_failed_scene_info'] \

            merged_info = value['MR_violation_failed_scene_info'] \
                          + value['MR_VT_failed_scene_info']

            # item["failed_map_key"] = {item['scene'] for item in merged_info}
            item["failed_map_count"] = {_name: 0 for _name in item["scene"]}
            for _i in merged_info:
                item["failed_map_count"][_i["scene"]] += 1
            _total_units = item["pos_unit"] + item["neg_unit"]
            item["failed_map_ratio"] = {k: item["failed_map_count"][k] / (_total_units / len(item["scene"])) for k in
                                        item["failed_map_count"].keys()}
            # "legal_group_percent": value["legal_unit_percent"],
            # "legal_case_percent": value["legal_case_percent"],
            # "MR_relation_finding_percent": value["MR_relation_finding_percent"],
            # legal_cases = {"t": pos_case, "f": neg_case}
            # legal_units = {"t": pos_unit, "f": neg_unit}
            # MR_relation_finding = {"t": pos_MR_finding, "f": neg_MR_finding}

            # item["distinct failure group"] = set()
            # item["distinct num"] = 0

            tmp.append(item)

    for item in tmp:
        item_set, tmp_set = create_distinct_set(item, tmp, "failed_scene_info")
        final_set = item_set - tmp_set
        item["failure group num"] = len(item_set)
        item["distinct failure group"] = final_set
        item["distinct failure group num"] = len(final_set)

        item_set, tmp_set = create_distinct_set(item, tmp, "MR_violation_failed_scene_info")
        final_set = item_set - tmp_set
        item["MR violation group num"] = len(item_set)
        item["MR violation distinct failure group"] = final_set
        item["MR violation distinct failure group num"] = len(final_set)

        item_set, tmp_set = create_distinct_set(item, tmp, "MR_transmission_failed_scene_info")
        final_set = item_set - tmp_set
        item["MR transmission failure group num"] = len(item_set)
        item["MR transmission distinct failure group"] = final_set
        item["MR transmission distinct failure group num"] = len(final_set)

        item_set, tmp_set = create_distinct_set(item, tmp, "MR_VT_failed_scene_info")
        final_set = item_set - tmp_set
        item["MR VT group num"] = len(item_set)
        item["MR VT distinct failure group"] = final_set
        item["MR VT distinct failure group num"] = len(final_set)

    df1 = pd.DataFrame(data=tmp)
    df1.sort_values(axis=0, inplace=True, by=["layer", "name", "agent"])
    df2 = df1[
        # ["layer", "name", "agent", "pos_case", "neg_case", "legal_case_percent", "pos_unit", "neg_unit",
        #  "legal_unit_percent", "pos_MR_finding", "neg_MR_finding", "MR_relation_finding_percent",
        #  "distinct failure group num", "MR violation distinct failure group num",
        #  "MR transmission distinct failure group num", "MR VT distinct failure group num", "failure group num",
        #  "MR violation group num", "MR transmission failure group num", "MR VT group num"]
        ["layer", "name", "agent", "pos_case", "neg_case", "pos_unit", "neg_unit",
         "pos_MR_finding", "neg_MR_finding", 'pos_MR_violation', 'neg_MR_violation',
         "distinct failure group num", "MR violation distinct failure group num",
         "MR transmission distinct failure group num", "MR VT distinct failure group num", "failure group num",
         "MR violation group num", "MR transmission failure group num", "MR VT group num"]
    ]
    # df1.set_index(["layer","name","agent"]).sort_index()
    df_base = df1[
        ["layer", "name_merge", "name", "agent", "pos_case", "neg_case", "pos_unit", "neg_unit",
         "pos_MR_finding", "neg_MR_finding", 'pos_MR_violation', 'neg_MR_violation',
         "MR violation group num", "MR transmission failure group num", "MR VT group num", ]
    ]

    # Total long EXCEL (without sum)
    _df3 = df_base.copy().groupby(["layer", "name", "agent"]).sum()
    get_statistics(_df3)
    df3 = pd.pivot_table(_df3, index=["layer", "name"],
                         values=["case_statistics_info", "group_info"],
                         columns=["agent"], aggfunc=lambda x: ' '.join(x))
    df3.rename(columns={"case_statistics_info": "Failure", "group_info": "Violation"}, inplace=True)
    # df3.sort_index(level="agent", axis="columns", inplace=True)

    # Total short EXCEL
    _df4 = df_base[["pos_case", "neg_case", "pos_unit", "neg_unit",
                    "pos_MR_finding", "neg_MR_finding", 'pos_MR_violation', 'neg_MR_violation',
                    "MR violation group num", "MR transmission failure group num", "MR VT group num"]]
    _df4.loc['Row_sum'] = _df4.apply(lambda x: x.sum())
    get_statistics(_df4)
    df4 = _df4.iloc[[-1]][["case_statistics_info", "group_info", "violation_info", "violation_type_info"]]
    df4.rename(columns={"case_statistics_info": "Failure", "group_info": "GroupPass", "violation_info": "Violation"},
               inplace=True)
    df4["pure violation"] = get_pure_violation_value(_df4)

    # Layer EXCEL
    _df5 = df_base.groupby("layer").sum()
    get_statistics(_df5)
    df5 = pd.pivot_table(_df5, index=["layer"],
                         values=["case_statistics_info", "group_info", "violation_info", "violation_type_info"],
                         aggfunc=lambda x: ' '.join(x))
    df5.rename(columns={"case_statistics_info": "Failure", "group_info": "GroupPass", "violation_info": "Violation"},
               inplace=True)
    df5["pure violation"] = get_pure_violation_value(_df5)

    # MR EXCEL
    _df6 = df_base.groupby("name_merge").sum()
    get_statistics(_df6)
    df6 = pd.pivot_table(_df6, index=["name_merge"],
                         values=["case_statistics_info", "group_info", "violation_info", "violation_type_info"],
                         aggfunc=lambda x: ' '.join(x))
    df5.rename(columns={"case_statistics_info": "Failure", "group_info": "GroupPass", "violation_info": "Violation"},
               inplace=True)
    # Total long EXCEL (sum part)
    # agent total
    _df7 = df_base.copy().groupby(["layer", "name"]).sum()
    get_statistics(_df7)
    df7 = pd.pivot_table(_df7, index=["layer", "name"],
                         values=["case_statistics_info", "group_info", "violation_info", "violation_type_info"],
                         aggfunc=lambda x: ' '.join(x))
    df7.rename(columns={"case_statistics_info": "Failure", "group_info": "GroupPass", "violation_info": "Violation",
                        "violation_type_info": "violation type info"}, inplace=True)
    # df3.sort_index(level="agent", axis="columns",inplace =True)

    _df8 = df_base.copy().groupby(["layer", "name_merge"]).sum()
    get_statistics(_df8)
    df8 = pd.pivot_table(_df8, index=["layer", "name_merge"],
                         values=["case_statistics_info", "group_info", "violation_info", "violation_type_info"],
                         aggfunc=lambda x: ' '.join(x))
    df8.rename(columns={"case_statistics_info": "Failure", "group_info": "GroupPass", "violation_info": "Violation",
                        "violation_type_info": "violation type info"}, inplace=True)

    _df9 = df_base.copy().groupby(["layer", "name_merge", "agent"]).sum()
    get_statistics(_df9)
    df9 = pd.pivot_table(_df9, index=["layer", "name_merge"],
                         values=["case_statistics_info", "group_info", "violation_info", "violation_type_info"],
                         columns=["agent"],
                         aggfunc=lambda x: ' '.join(x))
    df9.rename(columns={"case_statistics_info": "Failure", "group_info": "GroupPass", "violation_info": "Violation",
                        "violation_type_info": "violation type info"}, inplace=True)

    column_names = ["layer", "name", "agent", "scene", "failure_count", "failure_rate", "name_idx", "layer_idx"]
    origin_df10_table = []
    for iter_item in tmp:
        f_count = list(iter_item["failed_map_count"].items())
        f_rate = list(iter_item["failed_map_ratio"].items())
        if len(f_count) != len(f_rate):
            print("Warning! f_count != f_rate  !!!!!!!!")
        for idx in range(len(f_count)):
            origin_df10_table.append(
                [iter_item["layer"], iter_item["name"], int(iter_item["agent"]),
                 f_count[idx][0], f_count[idx][1], f_rate[idx][1],
                 print_mr_name_list.index(iter_item["name"]), print_layer_name_list.index(iter_item["layer"])])

    _df10_base = pd.DataFrame(origin_df10_table, columns=column_names)
    df10_MR_per_scene_failure_count = pd.pivot_table(
        _df10_base.copy().groupby(["scene", "layer", "name", "agent"]).sum(),
        index=["scene", "name_idx", "layer", "name"],
        values=["failure_count"],
        columns=["agent"],
    )
    df10_MR_per_scene_failure_count.sort_index(level=("scene", "name_idx"), inplace=True)

    # scenes = df10_MR_per_scene_failure_count.index.get_level_values(0).unique()
    # for scene in scenes:
    #     split = df10_MR_per_scene_failure_count.xs(scene)
    #     agent_list = sorted(list(split.columns.get_level_values(1).unique()))
    #
    #     MyPlot.draw_mutiple_bar_plot_for_scenes(split, scene + ".png",print_mr_name_list,agent_list)

    df10_MR_per_scene_failure_rate = pd.pivot_table(
        _df10_base.copy().groupby(["scene", "layer", "name", "agent"]).mean(),
        index=["scene", "name_idx", "layer", "name"],
        values=["failure_rate"],
        columns=["agent"],
    )
    df10_MR_per_scene_failure_rate.sort_index(level=("scene", "name_idx"), inplace=True)

    df11_Layer_per_scene_failure_count = pd.pivot_table(
        _df10_base.copy().groupby(["scene", "layer", "layer_idx", "agent"]).sum(),
        index=["scene", "layer", "layer_idx"],
        values=["failure_count"],
        columns=["agent"],
    )

    df11_Layer_per_scene_failure_count.sort_index(level=("scene", "layer_idx"), inplace=True)

    df11_Layer_per_scene_failure_rate = pd.pivot_table(
        _df10_base.copy().groupby(["scene", "layer", "layer_idx", "agent"]).mean(),
        index=["scene", "layer", "layer_idx"],
        values=["failure_rate"],
        columns=["agent"],
    )

    df11_Layer_per_scene_failure_rate.sort_index(level=("scene", "layer_idx"), inplace=True)

    # =================================================================================================
    data_new_prepared = []
    for k, values in data_dict.items():
        for v in values:
            failed_scene_info = v['failed_scene_info']
            for item in failed_scene_info:
                item["name"] = k
                item["agent"] = item['agent_num']
                item["origin"] = 1 if item['failed_type'] == 0 else 0
                item["follow"] = 1 if item['failed_type'] != 0 else 0
                item['violation'] = 1 if any([item['seed'] == i['seed'] for i in v["MR_violation_failed_scene_info"]]) \
                                         or any([item['seed'] == i['seed'] for i in v["MR_VT_failed_scene_info"]]) \
                    else 0
            data_new_prepared.extend(failed_scene_info)
    # ASE new requirment:
    df12_origin = pd.DataFrame(data=data_new_prepared)
    df12_big_table = pd.pivot_table(
        df12_origin.copy(),
        columns=["name", "scene", "agent", ],
        values=['origin', 'follow', "violation"],
        index=['seed'],
        aggfunc=[sum],
        fill_value=0,
    ).reorder_levels([0, 2, 3, 4, 1], axis=1).sort_index(axis='columns', level='name')

    df13_big_table = pd.pivot_table(
        df12_origin.copy(),
        columns=["name", "scene", "agent", ],
        values=['follow', "violation"],
        index=['seed'],
        aggfunc=[sum],
        fill_value=0,
    ).reorder_levels([0, 2, 3, 4, 1], axis=1).sort_index(axis='columns', level='name')

    return [df1, df2, df3, df4, df5, df6, df7, df8, df9,
            df10_MR_per_scene_failure_count, df10_MR_per_scene_failure_rate,
            df11_Layer_per_scene_failure_count, df11_Layer_per_scene_failure_rate, df12_big_table, df13_big_table]


def get_pure_violation_value(_df4):
    # return _df4[["pos_unit", "neg_unit", "pos_MR_finding"]].apply(
    #     lambda x: "{:d}/{:d}:({:.2%})".format(x["pos_unit"] + x["neg_unit"], x["pos_MR_finding"],
    #                                           x["pos_MR_finding"] / (x["pos_unit"] + x["neg_unit"])), axis=1)
    return _df4[["pos_unit", "neg_unit", "pos_MR_finding"]].apply(
        lambda x: "{:d}/{:d}:({:.2%})".format(x["pos_MR_finding"], x["pos_unit"] + x["neg_unit"],
                                              x["pos_MR_finding"] / (x["pos_unit"] + x["neg_unit"])), axis=1)


def get_statistics(df5):
    # df5["case_statistics_info"] = df5[["pos_case", "neg_case"]].apply(
    #     lambda x: "{:d}/{:d}:({:.2%})".format(x["pos_case"], x["neg_case"],
    #                                           x["pos_case"] / (x["pos_case"] + x["neg_case"])), axis=1)
    # df5["group_info"] = df5[["pos_unit", "neg_unit"]].apply(
    #     lambda x: "{:d}/{:d}:({:.2%})".format(x["pos_unit"], x["neg_unit"],
    #                                           x["pos_unit"] / (x["pos_unit"] + x["neg_unit"])), axis=1)
    # df5["violation_type_info"] = df5[["pos_MR_finding", "MR transmission failure group num", "MR VT group num"]].apply(
    #     lambda x: "{:d}/{:d}/{:d}".format(x["pos_MR_finding"], x["MR transmission failure group num"],
    #                                       x["MR VT group num"]), axis=1)

    df5["case_statistics_info"] = df5[["pos_case", "neg_case"]].apply(
        lambda x: "{:d}/{:d}:({:.2%})".format(x["neg_case"], (x["pos_case"] + x["neg_case"]),
                                              x["neg_case"] / (x["pos_case"] + x["neg_case"])), axis=1)
    df5["group_info"] = df5[["pos_unit", "neg_unit"]].apply(
        lambda x: "{:d}/{:d}:({:.2%})".format(x["neg_unit"], (x["pos_unit"] + x["neg_unit"]),
                                              x["neg_unit"] / (x["pos_unit"] + x["neg_unit"])), axis=1)
    # df5["violation_info"] = df5[["pos_MR_violation", "neg_MR_violation"]].apply(
    #     lambda x: "{:d}/{:d}:({:.2%})".format(
    #         x["neg_MR_violation"],
    #         (x["pos_MR_violation"] + x["neg_MR_violation"]),
    #         x["neg_MR_violation"] / (x["pos_MR_violation"] + x["neg_MR_violation"])),
    #     axis=1)

    df5["violation_info"] = df5[
        ["pos_unit", "neg_unit", "pos_MR_finding", "MR VT group num"]].apply(
        lambda x: "{:d}/{:d}:({:.2%})".format(
            x["pos_MR_finding"] + x["MR VT group num"],
            (x["pos_unit"] + x["neg_unit"]),
            (x["pos_MR_finding"] + x["MR VT group num"]) / (x["pos_unit"] + x["neg_unit"])),
        axis=1)

    df5["violation_type_info"] = df5[["pos_MR_finding", "MR transmission failure group num", "MR VT group num"]].apply(
        lambda x: "{:d}/{:d}/{:d}".format(x["pos_MR_finding"], x["MR transmission failure group num"],
                                          x["MR VT group num"]), axis=1)


def create_distinct_set(item, tmp, key):
    tmp_set = set()
    for other_items in tmp:
        if other_items == item:
            pass
        else:
            for i in other_items[key]:
                tmp_set.add((i["seed"], i["scene"], i["agent_num"]))
    item_set = set()
    for i in item[key]:
        item_set.add((i["seed"], i["scene"], i["agent_num"]))
    return item_set, tmp_set


def create_hit_set(list_origin, int_type) -> dict:
    total_dict = {}
    agent_merged_dict = {}
    for MR_specific_agent_item in list_origin:
        if int_type == 0:  # MR验证失效的所有测试用例组(test group)

            # statistics 1
            # tmp_set1 = {(i["seed"], i["scene"], MR_specific_agent_item["agent"]) for i in
            #             MR_specific_agent_item["MR_transmission_failed_scene_info"]}
            # tmp_set2 = {(i["seed"], i["scene"], MR_specific_agent_item["agent"]) for i in
            #             MR_specific_agent_item["MR_violation_failed_scene_info"]}
            # tmp_set3 = {(i["seed"], i["scene"], MR_specific_agent_item["agent"]) for i in
            #             MR_specific_agent_item["MR_VT_failed_scene_info"]}
            # tmp_set = set().union(tmp_set1, tmp_set2, tmp_set3)  # neg_unit num (should be)

            # statistics 2
            # tmp_set2 = {(i["seed"], i["scene"], MR_specific_agent_item["agent"]) for i in
            #             MR_specific_agent_item["MR_violation_failed_scene_info"]}
            #
            # tmp_set = tmp_set2  # violation num (should be)

            # statistics 3
            # tmp_set1 = {(i["seed"], i["scene"], MR_specific_agent_item["agent"]) for i in
            #             MR_specific_agent_item["MR_transmission_failed_scene_info"]}
            tmp_set2 = {(i["seed"], i["scene"], MR_specific_agent_item["agent"]) for i in
                        MR_specific_agent_item["MR_violation_failed_scene_info"]}
            tmp_set3 = {(i["seed"], i["scene"], MR_specific_agent_item["agent"]) for i in
                        MR_specific_agent_item["MR_VT_failed_scene_info"]}
            tmp_set = set().union(tmp_set2, tmp_set3)  # neg_unit num (should be)

        elif int_type == 1:  # 触发 transmission_failed 的测试用例组
            # statistics 1
            # tmp_set = {(i["seed"], i["scene"]) for i in MR_specific_agent_item["failed_scene_info"]}

            # statistics 2
            tmp_set = {(i["seed"], i["scene"], MR_specific_agent_item["agent"]) for i in
                       MR_specific_agent_item["failed_scene_info"]}
        else:
            raise ValueError("int_type illegal")

        # print(MR_specific_agent_item["name"], MR_specific_agent_item["agent"], len(tmp_set))

        if MR_specific_agent_item["name"] in agent_merged_dict.keys():
            agent_merged_dict[MR_specific_agent_item["name"]] = \
                set().union(agent_merged_dict[MR_specific_agent_item["name"]], tmp_set)
        else:
            agent_merged_dict[MR_specific_agent_item["name"]] = tmp_set
        # print(len(agent_merged_dict[MR_specific_agent_item["name"]]))
    for key in agent_merged_dict.keys():
        for value in agent_merged_dict[key]:
            if value in total_dict.keys():
                total_dict[value].append(key)
            else:
                total_dict[value] = [key]

    return total_dict


def create_compare_table_file_intense(filename: str, data_dict: dict, merge_type: str):
    tmp = []
    for key in data_dict.keys():
        for value in data_dict[key]:
            j = json.loads(value["mt_para"])
            item = value

            # total
            if merge_type == "MR":
                item["name"] = name_para_merge_converter(key)
            elif merge_type == "Layer":
                item["name"] = item["layer"]
            elif merge_type == None:
                item["name"] = key

            item["agent"] = j["ratio"]
            item["pos_case"] = value["legal_cases"]["t"]
            item["neg_case"] = value["legal_cases"]["f"]
            item["pos_unit"] = value["legal_units"]["t"]
            item["neg_unit"] = value["legal_units"]["f"]
            item["pos_MR_finding"] = value["MR_relation_finding"]["t"]
            item["neg_MR_finding"] = value["MR_relation_finding"]["f"]

            tmp.append(item)

    if merge_type == "MR":
        order_list = print_mr_name_list
    elif merge_type == "Layer":
        order_list = print_layer_name_list
    elif merge_type == None:
        order_list = []

    GenerateDistinitSetInfo(0, filename, tmp, order_list, merge_type)

    # GenerateDistinitSetInfo(1, filename, tmp, order_list, merge_type)


def name_para_merge_converter(key):
    result = None
    # if "Rotation" in key:
    #     result = "mr_1_Rotation"
    # elif "Mirror" in key:
    #     result = "mr_4_Mirror"
    # elif "AddWindowedPriorityProbe" in key:
    #     result = "AddWindowedPriorityProbe"
    # else:
    #     result = key
    idx = max(key.rfind('-'), key.rfind('_'))
    if idx == -1:
        result = key
    else:
        result = key[idx + 1:]
    return result


def GenerateDistinitSetInfo(idx, filename, list_origin, order_list, merge_type):
    total_dict_case = create_hit_set(list_origin, idx)

    # name_set = {s for values in total_dict_case.values() for s in values}
    name_set = order_list

    base_addr = os.path.dirname(os.path.abspath(filename[idx]))
    # 2023.7.16 new requirments from Xiaoyi Zhang
    MR_vs_Other_df = MR_comparison_1vOther(base_addr, idx, merge_type, order_list, total_dict_case)
    MR_comparison_1v1(base_addr, idx, merge_type, order_list, total_dict_case, MR_vs_Other_df)



    # print(total_dict_case)
    result_dict = {}

    for key in total_dict_case.keys():
        for mr_name in total_dict_case[key]:
            if mr_name in result_dict.keys():
                result_dict[mr_name][len(total_dict_case[key]) - 1] += 1
            else:
                result_dict[mr_name] = [0 for i in range(len(name_set))]
                result_dict[mr_name][len(total_dict_case[key]) - 1] += 1
    print(result_dict)
    # empty check
    for mr_name in order_list:
        if mr_name not in result_dict.keys():
            result_dict[mr_name] = [0 for i in range(len(name_set))]

    MyPlot.DistinctScenesComparisionCount(result_dict, filename[idx], idx, order_list)

    MyPlot.DistinctScenesComparisionTotalCount(result_dict, filename[idx], idx, 0, order_list)

    MyPlot.DistinctScenesComparisionTotalCount(result_dict, filename[idx], idx, 1, order_list)

    result_list = []

    for key in result_dict:
        agent_para_set = {json.loads(x['mt_para'])['ratio'] for x in list_origin if x["name"] == key}
        if len(agent_para_set) == 0:
            print(f"seem the {key} do not exist in data, please confirm the title of key is correct in "
                  " the variable 'print_mr_name_list'")

        selected_key = [x for x in list_origin if x["name"] == key]
        repeated_num = len(selected_key) / len(agent_para_set)
        total_unit = 0
        for x in selected_key:
            total_unit += x["pos_unit"] + x["neg_unit"]
        result_list.append(
            {"name": key, "info": str(result_dict[key]), "total": sum(result_dict[key]),
             "total_unit": total_unit / repeated_num})
    df = pd.DataFrame(result_list)
    df.to_csv(filename[idx])

    for item in result_list:
        key = item["name"]
        # TODO : should I devide the x / item["total_unit"] / repeated_num ?
        result_dict[key] = [x / item["total_unit"] for x in result_dict[key]]
    MyPlot.DistinctScenesComparisionRatio(result_dict, filename[idx], idx, order_list)


def MR_comparison_1v1(base_address, type_str, merge_type, order_list, total_dict_case,
                      MRvsOtherDataFrame: Optional[pd.DataFrame] = None):
    result_dict = {}
    for k in [(x, y) for x in order_list for y in order_list]:
        result_dict[k] = 0
    for key, MRs in total_dict_case.items():
        for k in [(x, y) for x in MRs for y in MRs]:
            result_dict[k] += 1
    # # Get keys (as indices for output) and values as arrays
    # idx = np.array(result_dict.keys())
    # vals = np.array(result_dict.values())
    # # Get dimensions of output array based on max extents of indices
    # dims = idx.max(0) + 1
    # # Setup output array and assign values into it indexed by those indices
    # out = np.zeros(dims, dtype=vals.dtype)
    # out[idx[:, 0], idx[:, 1]] = vals
    s = pd.Series(result_dict)
    df1 = s.unstack()
    df1 = df1[order_list]
    df1.index.name = "MRs"
    # df1.reset_index(inplace=True)
    # df1 = df1.sort_index().sort_index(axis=1)
    df1 = df1.reindex(order_list)

    print(df1)

    df1.to_excel(f"{base_address}/Two_{merge_type}_Comparison_count_{type_str}.xlsx")

    result_dict2 = {(x, y): result_dict[x, y] * 1.0 / result_dict[x, x] if result_dict[x, x] is not 0 else 1
                    for x in order_list for y in order_list}
    df2 = pd.Series(result_dict2).unstack()

    df2 = df2[order_list]
    df2.index.name = "MRs"
    df2 = df2.reindex(order_list)
    print(df2)

    if MRvsOtherDataFrame is not None:
        a = MRvsOtherDataFrame['MR_contains_based_ratio'].tolist()
        print(MRvsOtherDataFrame['MR_contains_based_ratio'])
        # add column
        df2 = df2.assign(Others=a)

        head_data = pd.DataFrame({'Others': ["Others"]})  # 插入到头部的数据
        tail_data = pd.DataFrame({'Others': [1.0]})  # 插入到尾部的数据
        middle_data = pd.DataFrame({'Others': MRvsOtherDataFrame['others_based_ratio']})
        new_row = pd.DataFrame(
            pd.concat([middle_data, tail_data]).reset_index(drop=True)
        ).transpose()
        # new_row.index.name = tuple("Others")
        new_row.columns = df2.columns
        # add row
        df2 = df2.append(new_row, ignore_index=True)
        order_list_new = list(order_list)
        order_list_new.append("Others")
        print(df2)
        df2.index = order_list_new
        print(df2)
        # df2 = df2[order_list_new]
        # df2.index.name = "MRs"
        # df2 = df2.reindex(order_list_new)
        # print(df2)

    df2.to_excel(f"{base_address}/Two_{merge_type}_Comparison_ratio_{type_str}.xlsx")


def MR_comparison_1vOther(base_address, type_str, merge_type, order_list, total_dict_case):
    @dataclass
    class MRvsOtherData():
        MR_contains: int
        MR_and_others: int
        others_contains: int

    result_dict = {}
    for k in order_list:
        result_dict[k] = MRvsOtherData(0, 0, 0)
    for key, MRs in total_dict_case.items():
        for k in MRs:
            if len(MRs) >= 2:
                result_dict[k].MR_and_others += 1

            result_dict[k].MR_contains += 1

    for k in order_list:
        for key, MRs in total_dict_case.items():
            if not ((k in MRs) and (len(MRs) == 1)):
                result_dict[k].others_contains += 1

    df1 = pd.DataFrame(
        [{'MR': k, 'MR_contains': v.MR_contains, 'MR_and_others': v.MR_and_others, 'others_only': v.others_contains} for
         k, v in result_dict.items()])

    # df1.columns = order_list
    # df1.index.name = tuple(["MR", "MR_contains", "MR_and_others", "other_only"])
    # df1 = df1.reset_index()
    print(df1)
    df1.to_excel(f"{base_address}/1vsOther_{merge_type}_Comparison_count_{type_str}.xlsx")

    df2 = pd.DataFrame(
        [{'MR': k,
          'MR_contains_based_ratio': v.MR_and_others * 1.0 / v.MR_contains if v.MR_contains is not 0 else np.nan,
          'others_based_ratio': v.MR_and_others * 1.0 / v.others_contains if v.others_contains is not 0 else np.nan} for
         k, v in result_dict.items()])
    return df2



def GetViolationThresholdFlows(base_path: str):
    MyPlot.ViolationThresholdFlows(base_path, "mr", "Ratio")
    MyPlot.ViolationThresholdFlows(base_path, "layer", "Ratio")
    MyPlot.ViolationThresholdFlows(base_path, "mr", "Absolute")
    MyPlot.ViolationThresholdFlows(base_path, "layer", "Absolute")
