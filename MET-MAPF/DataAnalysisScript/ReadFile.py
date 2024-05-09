# !/usr/bin/python
# -*- coding: UTF-8 -*-

import os, json, sys, getopt
import ProducerConsumerPattern
from multiprocessing import JoinableQueue, Process
from functools import partial

import TableAndGraph as tg
import MyPlot as mp
from pathlib import Path
import random

convert_name_dict = {
    "Rotation": "rot",

    "Mirror": "flip",
    "DecreaseObstacle": "rmObs",
    "IncreaseObstacleRandom": "addObs",

    "Exchange": "swap",
    "IncreaseObstacleNoninteraction": "addIrrObs",
    "ShrinkStartPos": 'shorten',

    "AddAgent": "addAgent",
    "DeleteAgent": "rmAgent",
    "PriorityExchange": "priorex",

    # "AddRandomPriorityProbe": 'probe',
    # "AddWindowedPriorityProbe": 'windowProbe',
    "AddWindowedPriorityProbe": 'probe',


    #============================#
    #new name fix
    'IncObstacleFree' : "addIrrObs",
    'AddPriorityProbe' : "probe",
    'IncObstacleRan': "addObs"

}


def usage():
    print("Input options:")
    print("-h, --help : help doc")
    print("-i, --input : input base path")
    print("-o, --output : output file name")
    print("-l, --loop : find all possible folders and run scripts")


def main():
    try:
        opts, args = getopt.getopt(sys.argv[1:], "hli:o:", ["help", "loop", "input=", "output="])
    except getopt.GetoptError as err:
        # print help information and exit:
        print(err)  # will print something like "option -a not recognized"
        usage()
        sys.exit(2)

    # new change, if you don't want to merge, change the name here!
    base_path = None
    loop_mode = False
    j_data_filename = "data_map.json"
    for o, a in opts:
        if o in ("-h", "--help"):
            usage()
            sys.exit()
        elif o in ("-o", "--output"):
            j_data_filename = a
        elif o in ("-i", "--input"):
            base_path = a
        elif o in ("-l", "--loop"):
            loop_mode = True
        else:
            assert False, "unhandled option"

    if not os.path.exists(base_path):
        print("illegal input path, please check input path string")
        exit(-1)


    # base_path = "/home/hiccup/Appendix/PIBT-Output-V2/6.14.2023/MetamorphicTestingData-origin/"


    if not loop_mode:
        execution(base_path, j_data_filename)
    else:
        input_list = []

        for dir in os.listdir(base_path):
            dir_path = os.path.join(base_path, dir)
            print(dir_path)
            if os.path.isdir(dir_path):
                input_list.append(partial(execution, dir_path, j_data_filename))
                # input_list.append(partial(random.randint,3,5) )
        q = JoinableQueue()
        p1 = Process(target=ProducerConsumerPattern.producer, args=('Producer1', input_list, q))
        p1.start()

        consumers = []
        num = 6
        for i in range(num):
            c = Process(target=ProducerConsumerPattern.consumer, args=("Consumer_" + str(i), q))
            c.daemon = True
            c.start()
            consumers.append(c)

        print("Multi-thread start")
        p1.join()
        print("Multi-thread complete")
        tg.GetViolationThresholdFlows(base_path)



# new cahnge, if you don't want to merge, change the name here!
# 1.3.2023
def GetPrintName(mr_name: str) -> str:
    # True means same MR with different parameters will be merged.
    merge_MRs = True
    # merge_MRs = False

    name = mr_name.split('-')
    if len(name) == 3:
        for key in convert_name_dict.keys():
            # if (mr_name in key):
            if name[2] == key:
                if merge_MRs:
                    return convert_name_dict[key]
                else:
                    return convert_name_dict[key] + "~" + name[1]

    return "random"


def GetLevel(mr_name: str) -> str:
    # l1 = "1 - Environment Layer"
    # l2 = "2 - Single-Agent Layer"
    # l3 = "3 - Agent-Group Layer"
    # l4 = "4 - Interaction Layer"

    # l1 = "1 - Environment Layer"
    # l2 = "2 - Single-Agent Layer"
    # l3 = "3 - Multi-Agent Layer"

    l1 = "EL"
    l2 = "SAL"
    l3 = "MAL"

    name_level_dict = {
        "mr_1-p1-Rotation": l1,
        "mr_1-p2-Rotation": l1,
        "mr_1-p3-Rotation": l1,
        "mr_2-p1-Mirror": l1,
        "mr_2-p2-Mirror": l1,
        "mr_2-p3-Mirror": l1,
        "mr_3-p1-DecreaseObstacle": l1,
        "mr_3-p2-DecreaseObstacle": l1,
        "mr_3-p3-DecreaseObstacle": l1,
        "mr_4.1-p1-IncreaseObstacleRandom": l1,
        "mr_4.1-p2-IncreaseObstacleRandom": l1,
        "mr_4.1-p3-IncreaseObstacleRandom": l1,
        "mr_1+2-p1-Reshape": l1,
        "mr_1+2-p2-Reshape": l1,
        "mr_1+2-p3-Reshape": l1,

        # old
        "mr_4-p1-IncreaseObstacle": l1,
        "mr_4-p2-IncreaseObstacle": l2,
        "mr_4-p3-IncreaseObstacle": l2,

        "mr_4.2-p1-IncreaseObstacleNoninteraction": l2,
        "mr_4.2-p2-IncreaseObstacleNoninteraction": l2,
        "mr_4.2-p3-IncreaseObstacleNoninteraction": l2,
        "mr_5-p1-Exchange": l2,
        "mr_5-p2-Exchange": l2,
        "mr_5-p3-Exchange": l2,
        "mr_6-p1-ShrinkStartPos": l2,
        "mr_6-p2-ShrinkStartPos": l2,
        "mr_6-p3-ShrinkStartPos": l2,

        "mr_7-p1-PriorityExchange": l3,
        "mr_7-p2-PriorityExchange": l3,
        "mr_7-p3-PriorityExchange": l3,
        "mr_8-p1-DeleteAgent": l3,
        "mr_8-p2-DeleteAgent": l3,
        "mr_8-p3-DeleteAgent": l3,
        "mr_9-p1-AddAgent": l3,
        "mr_9-p2-AddAgent": l3,
        "mr_9-p3-AddAgent": l3,
        "mr_10-p1-AddWindowedPriorityProbe": l3,
        "mr_10-p2-AddWindowedPriorityProbe": l3,
        "mr_10-p3-AddWindowedPriorityProbe": l3,
        "mr_11-p1-AddRandomPriorityProbe": l3,
        "mr_11-p2-AddRandomPriorityProbe": l3,
        "mr_11-p3-AddRandomPriorityProbe": l3,

        # updated for names:
        "mr_10-p1-AddPriorityProbe": l3,
        "mr_10-p2-AddPriorityProbe": l3,
        "mr_10-p3-AddPriorityProbe": l3,

        "mr_4.1-p1-IncObstacleRan": l1,
        "mr_4.1-p2-IncObstacleRan": l1,
        "mr_4.1-p3-IncObstacleRan": l1,

        "mr_4.2-p1-IncObstacleFree": l2,
        "mr_4.2-p2-IncObstacleFree": l2,
        "mr_4.2-p3-IncObstacleFree": l2,
    }

    name = mr_name.split('-')
    if len(name) == 3:
        for key in name_level_dict.keys():
            # if (mr_name in key):
            if name[2] in key:
                return name_level_dict[key]

    return "0 - None "


def GetMergedInfo(failed_info_list: list):
    result = []
    for item in failed_info_list:
        result.extend(item)
    if len(result):
        if 'failed_type' in result[0].keys():
            result = set([(item['seed'], item['scene'], item['agent_num'], item['failed_type']) for item in result])
            result = [{'seed': item[0], 'scene': item[1], 'agent_num': item[2], 'failed_type': item[3]} for item in
                      result]
        else:
            result = set([(item['seed'], item['scene'], item['agent_num']) for item in result])
            result = [{'seed': item[0], 'scene': item[1], 'agent_num': item[2]} for item in result]
    return result


def GetUnionResult(_data_origin: dict):
    _data = {}
    agent_set = {value['agent_num'] for value_list in _data_origin.values() for value in value_list}
    for mr_name in _data_origin.keys():
        _data[mr_name] = []
        _tmp_data_group = [[v for v in _data_origin[mr_name] if v['agent_num'] == agent_num] for
                           agent_num in agent_set]
        for data_list_prepared_merged in _tmp_data_group:
            _add_dict = {}
            total_cases = data_list_prepared_merged[0]['legal_cases']['t'] + \
                          data_list_prepared_merged[0]['legal_cases']['f']
            total_units = data_list_prepared_merged[0]['legal_units']['t'] + \
                          data_list_prepared_merged[0]['legal_units']['f']
            _add_dict['mt_para'] = data_list_prepared_merged[0]['mt_para']
            _add_dict['mr_para'] = data_list_prepared_merged[0]['mr_para']
            _add_dict['agent_num'] = data_list_prepared_merged[0]['agent_num']
            _add_dict['failed_scene_info'] = GetMergedInfo(
                [item['failed_scene_info'] for item in data_list_prepared_merged])
            _add_dict['MR_violation_failed_scene_info'] = GetMergedInfo(
                [item['MR_violation_failed_scene_info'] for item in data_list_prepared_merged])
            _add_dict['MR_transmission_failed_scene_info'] = GetMergedInfo(
                [item['MR_transmission_failed_scene_info'] for item in data_list_prepared_merged])
            _add_dict['MR_VT_failed_scene_info'] = GetMergedInfo(
                [item['MR_VT_failed_scene_info'] for item in data_list_prepared_merged])
            _add_dict['layer'] = data_list_prepared_merged[0]['layer']
            _add_dict['scene'] = data_list_prepared_merged[0]['scene']
            _add_dict['legal_cases'] = {"t": total_cases - len(_add_dict['failed_scene_info']),
                                        "f": len(_add_dict['failed_scene_info'])}
            _add_dict['legal_units'] = {
                "t": total_units - len(_add_dict['MR_violation_failed_scene_info']) - len(
                    _add_dict['MR_VT_failed_scene_info']),
                "f": len(_add_dict['MR_violation_failed_scene_info']) + len(_add_dict['MR_VT_failed_scene_info'])}

            _add_dict['MR_relation_finding'] = {
                "t": total_units - len(_add_dict['MR_VT_failed_scene_info']) - len(
                    _add_dict['MR_transmission_failed_scene_info']),
                "f": len(_add_dict['MR_VT_failed_scene_info']) + len(_add_dict['MR_transmission_failed_scene_info'])
            }

            _add_dict['MR_relation_finding_percent'] = _add_dict['MR_relation_finding']['t'] / total_units
            _add_dict['legal_case_percent'] = _add_dict['legal_cases']['t'] / total_cases
            _add_dict['legal_unit_percent'] = _add_dict['legal_units']['t'] / total_units

            _data[mr_name].append(_add_dict)

    return _data


def execution(base_path: str, j_data_filename: str):
    _data_total_algo = {}
    illegal_info = []
    statistics_type = 'add'
    # statistics_type = 'union'

    os.chdir(base_path)
    for path_mt in Path(base_path).rglob("*MT-para.json"):

        print(path_mt.name)
        print(path_mt.parent.name)
        print(path_mt)


        _statistics = {}

        with open(path_mt, 'r') as _mt_para_file:
            j_mt_para = json.load(_mt_para_file)

            print(json.dumps(j_mt_para, sort_keys=True, indent=4, separators=(',', ':')))
            print("========================================")
            for mr_info in j_mt_para["MR_list"]:
                print("mr-name:", mr_info["name"])
                print("mr-class-name:", mr_info["reference_class_name"])
                print("MR_para_str:", json.dumps(mr_info["MR_para_str"]))
                uuid_str = mr_info["uuid"]

            mt_para_str = j_mt_para["MT_hyper_param"]
            para_j = json.loads(mt_para_str)
            agent_num = int(para_j["ratio"])
            algorithm_str = str(para_j["algorithm"])
            print("agent num:{}, algorithm name: {}".format(para_j['ratio'],algorithm_str) )
            print("========================================")

            position = path_mt.name.find("MT-para.json")
            folder_name = path_mt.name[0:position]
            print("open folder:", folder_name)
            data_path = path_mt.parents[0] / folder_name / j_data_filename
            #check data path exist?
            if not os.path.exists(data_path):
                data_path = path_mt.parents[0] / (folder_name + '+' + uuid_str) / j_data_filename


            if algorithm_str not in _data_total_algo.keys():
                _data_total_algo[algorithm_str] = {}
            _data_single_algo = _data_total_algo[algorithm_str]

            try:
                with open(data_path, "r") as j_datafile:
                    j = json.load(j_datafile)
                    # print(j)

                    for group in j["group"]:
                        mr_para_str = group["param"]
                        # mr_name = group["name"] + "-" + group["type"]
                        mr_name = mr_info["name"] + "-" + mr_info["reference_class_name"]
                        mr_print_name = GetPrintName(mr_name)
                        pos_case = 0
                        neg_case = 0
                        pos_unit = 0
                        neg_unit = 0
                        pos_MR_finding = 0
                        neg_MR_finding = 0
                        # pos_MR_violation = 0
                        # neg_MR_violation = 0

                        failed_scene_info = []
                        MR_violation_failed_scene_info = []
                        MR_transmission_failed_scene_info = []
                        MR_VT_failed_scene_info = []
                        for unit in group["test_units"]:
                            cases_passed = True
                            for idx, case in enumerate(unit["statistics"]["statistics"]):
                                # if idx == 0 then the case is original case, else is the ith follow case.
                                if (case["is_legal_execution"] == True):
                                    pos_case += 1
                                if (case["is_legal_execution"] == False):
                                    neg_case += 1
                                    failed_scene_info.append(
                                        {"seed": unit["seed"], "scene": unit["origin_map_name"], "agent_num": agent_num,
                                         "uuid": unit["uuid"], "failed_type": idx})
                                    cases_passed = False

                            if not unit["statistics"]["pass"]:
                                if cases_passed:
                                    pos_MR_finding += 1  # violation finding
                                    MR_violation_failed_scene_info.append(
                                        {"seed": unit["seed"], "scene": unit["origin_map_name"], "agent_num": agent_num,
                                         "uuid": unit["uuid"]})
                                else:
                                    neg_MR_finding += 1  # transimission && violation finding
                                    MR_VT_failed_scene_info.append(
                                        {"seed": unit["seed"], "scene": unit["origin_map_name"], "agent_num": agent_num,
                                         "uuid": unit["uuid"]})
                            else:
                                if not cases_passed:
                                    neg_MR_finding += 1  # transimission finding
                                    MR_transmission_failed_scene_info.append(
                                        {"seed": unit["seed"], "scene": unit["origin_map_name"], "agent_num": agent_num,
                                         "uuid": unit["uuid"]})

                            is_relation_satisfied = cases_passed & unit["statistics"]["pass"]
                            if is_relation_satisfied == True:
                                pos_unit += 1
                            else:
                                neg_unit += 1

                            # is_voliation_satisfied = unit["statistics"]["pass"]
                            # if is_voliation_satisfied:
                            #     pos_MR_violation += 1
                            # else:
                            #     neg_MR_violation += 1

                            neg_MR_violation = len(MR_violation_failed_scene_info) + len(
                                MR_transmission_failed_scene_info)
                            pos_MR_violation = pos_unit + neg_unit - neg_MR_violation

                            # neg_MR_violation = len(MR_transmission_failed_scene_info)
                            # pos_MR_violation = len(MR_violation_failed_scene_info) + len(MR_VT_failed_scene_info)

                        legal_cases = {"t": pos_case, "f": neg_case}
                        legal_units = {"t": pos_unit, "f": neg_unit}
                        legal_violations = {"t": pos_MR_violation, 'f': neg_MR_violation}
                        MR_relation_finding = {"t": pos_MR_finding, "f": neg_MR_finding}
                        if neg_unit != 0:
                            MR_relation_finding_percent = pos_MR_finding / neg_unit
                        else:
                            MR_relation_finding_percent = 0
                        legal_case_percent = pos_case / (pos_case + neg_case)
                        legal_unit_percent = pos_unit / (pos_unit + neg_unit)
                        legal_violation_percent = pos_MR_violation / (pos_MR_violation + neg_MR_violation)

                        if (False == _data_single_algo.__contains__(mr_print_name)):
                            _data_single_algo[mr_print_name] = []
                        _data_single_algo[mr_print_name].append(
                            {"mr_para": mr_para_str,
                             "mt_para": mt_para_str,
                             "algorithm_name": algorithm_str,
                             "agent_num": agent_num,
                             "legal_cases": legal_cases,
                             "legal_units": legal_units,
                             'legal_violations': legal_violations,
                             "MR_relation_finding": MR_relation_finding,
                             "MR_relation_finding_percent": MR_relation_finding_percent,
                             "legal_unit_percent": legal_unit_percent,
                             "legal_case_percent": legal_case_percent,
                             'legal_violation_percent': legal_violation_percent,
                             "failed_scene_info": failed_scene_info,
                             "MR_violation_failed_scene_info": MR_violation_failed_scene_info,
                             "MR_transmission_failed_scene_info": MR_transmission_failed_scene_info,
                             "MR_VT_failed_scene_info": MR_VT_failed_scene_info,
                             "layer": GetLevel(mr_name),
                             "scene": j_mt_para["cases_list"],
                             })
                        if not tg.specific_MR_name:
                            if GetPrintName(mr_name) not in tg.print_mr_name_list:
                                tg.print_mr_name_list.append(GetPrintName(mr_name))
                            if GetLevel(mr_name) not in tg.print_layer_name_list:
                                tg.print_layer_name_list.append(GetLevel(mr_name))

            except Exception as e:
                illegal_info += [(data_path, e)]

    if len(illegal_info) != 0:
        print("===========================")
        print("Crushed Info:")
        print(illegal_info)

    # data clean
    deleted_keys = []
    for algo_name, _data_single_algo in _data_total_algo.items():

        for mr_name in _data_single_algo.keys():
            if mr_name not in tg.print_mr_name_list:
                deleted_keys.append(mr_name)
        for key in deleted_keys:
            _data_single_algo.pop(key, None)

        if statistics_type == 'union':
            _data_single_algo = GetUnionResult(_data_single_algo)

        output_folder = "statistics_folder/{}".format(algo_name)
        save_path = os.path.join(base_path, output_folder)
        os.makedirs(save_path, exist_ok=True)

        output = json.dumps(_data_single_algo, indent=2, sort_keys=True)
        # print(output)
        with open(save_path + "/output.json", "w") as f:
            f.writelines(output)

        csv_filename = "output.csv"
        csv_filename2 = "output2.csv"
        csv_layer_filename = "output_layer.csv"

        tg.create_table_file(save_path + "/" + csv_filename, _data_single_algo)

        tg.create_table_file_intense(save_path + "/" + csv_filename2, _data_single_algo)

        tg.create_layer_table_file_intense(
            [(save_path + "/") + "output_layer" + str(s + 1) + ".csv" for s in range(14)], _data_single_algo, save_path)

        tg.create_compare_table_file_intense(
            [(save_path + "/") + "output_MR_compare" + str(s + 1) + ".csv" for s in range(2)], _data_single_algo, "MR")

        tg.create_compare_table_file_intense(
            [(save_path + "/") + "output_layer_compare" + str(s + 1) + ".csv" for s in range(2)],
            _data_single_algo, "Layer")

        # if len(illegal_info) != 0:
        #     print("===========================")
        #     print("Crushed Info:")
        #     print(illegal_info)


if __name__ == "__main__":
    # execute only if run as a script

    main()
