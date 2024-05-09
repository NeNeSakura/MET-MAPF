import matplotlib
import matplotlib as mpl
import matplotlib.pyplot as plt
import matplotlib.ticker
import numpy as np
from itertools import cycle
import os
import matplotlib.ticker as mtick
import pandas as pd
import json

plt.rcParams["font.family"] = "Times New Roman"

parameters = {
    'axes.labelsize': 16,
    'axes.titlesize': 16,
    "figure.titlesize": 16,
    "xtick.labelsize": 16,
    "ytick.labelsize": 16,
    "legend.fontsize": 16,
    "legend.title_fontsize": 16
    }
plt.rcParams.update(parameters)



def DistinctScenesComparisionCount(MR_info_dict: dict, filename: str, idx: int, print_mr_name_list: list):
    plt.rcParams['savefig.dpi'] = 100  # 图片像素
    plt.rcParams['figure.dpi'] = 100  # 分辨率
    plt.rcParams['figure.figsize'] = (12, 7)  # 设置figure_size尺寸

    x_axis = [i + 1 for i in range(len(MR_info_dict.keys()))]
    # lines = ["-", "--", ":","-.",]
    lines = ["-", "--", ":", ]
    markers = ["o", "^", "v", "s"]
    legend_setter = [{"linestyle": line, "marker": marker} for line in lines for marker in markers]
    linecycler = cycle(legend_setter)


    # for key in sorted(MR_info_dict.keys()):
    for key in print_mr_name_list:
        if key in MR_info_dict.keys():
            plt.plot(x_axis, MR_info_dict[key], **next(linecycler), label=key)
            # Be sure to only pick integer tick locations.
            plt.xticks(x_axis)
            # plt.gca().set_yticklabels(['{:.0f}\%'.format(x*100) for x in plt.gca().get_yticks()])


    if idx == 0:
        plt.title('Violation Failed Test Group Result Analysis')
    #     _ax.yaxis.set_major_formatter(matplotlib.ticker.PercentFormatter(xmax=1,decimals=2))
        pass
    elif idx == 1:
        plt.title('Constructing failed case Result Analysis ')
    else:
        raise ValueError("illegal idx para.")

    plt.legend()  # 显示图例
    # plt.legend(loc='lower left', bbox_to_anchor=(1.04, 0), borderaxespad=0)  # 显示图例

    # plt.xlabel('Numbers of MR hits in specific test group')
    # plt.ylabel('Numbers of test group not passed')
    plt.xlabel('Number of violated MRs at same source test cases')
    plt.ylabel('Number of test group')


    # 能不能定义个词汇， transmission caused 和 violation caused 的全集叫什么？？
    # not passed 我需要起名字！！！
    filename_png = os.path.splitext(filename)[0] + "_counts" + ".eps"
    plt.savefig(filename_png, bbox_inches="tight")
    # plt.show()
    plt.cla()
    plt.clf()
    plt.close()


def DistinctScenesComparisionRatio(MR_info_dict: dict, filename: str, idx: int, print_mr_name_list: list):
    plt.rcParams['savefig.dpi'] = 100  # 图片像素
    plt.rcParams['figure.dpi'] = 100  # 分辨率
    plt.rcParams['figure.figsize'] = (12, 7)  # 设置figure_size尺寸

    x_axis = [i + 1 for i in range(len(MR_info_dict.keys()))]
    # lines = ["-", "--", ":","-.",]
    lines = ["-", "--", ":", ]
    markers = ["o", "^", "v", "s"]
    legend_setter = [{"linestyle": line, "marker": marker} for line in lines for marker in markers]
    linecycler = cycle(legend_setter)
    if idx == 0:
        # plt.title('Violation Failed Test Group Result Analysis')
        pass
    elif idx == 1:
        plt.title('Constructing failed case Result Analysis ')
    else:
        raise ValueError("illegal idx para.")

    # for key in sorted(MR_info_dict.keys()):
    for key in print_mr_name_list:
        if key in MR_info_dict.keys():
            _ax = plt.plot(x_axis, MR_info_dict[key], **next(linecycler), label=key)
            # Be sure to only pick integer tick locations.
            plt.xticks(x_axis)
            # _ax.yaxis.set_major_formatter(matplotlib.ticker.PercentFormatter(xmax=1,decimals=2))
            plt.gca().set_yticklabels(['{:.2f}%'.format(x*100) for x in plt.gca().get_yticks()])


    plt.legend()  # 显示图例
    # plt.legend(loc='lower left', bbox_to_anchor=(1.04, 0), borderaxespad=0)  # 显示图例

    plt.xlabel('Number of violated MRs at same source test cases')
    plt.ylabel('Percentage of test group (%)')
    # plt.xlabel('Numbers of MR hits in specific test group')
    # plt.ylabel('Ratios of test group not passed')
    # 能不能定义个词汇， transmission caused 和 violation caused 的全集叫什么？？
    # not passed 我需要起名字！！！
    filename_png = os.path.splitext(filename)[0] + "_ratio" + ".eps"
    plt.savefig(filename_png, bbox_inches="tight")
    # plt.show()
    plt.cla()
    plt.clf()
    plt.close()


def DistinctScenesComparisionTotalCount(MR_info_dict: dict, filename: str, idx: int, GraphType:int, print_mr_name_list: list):
    plt.rcParams['savefig.dpi'] = 100  # 图片像素
    plt.rcParams['figure.dpi'] = 100  # 分辨率
    plt.rcParams['figure.figsize'] = (8, 8)  # 设置figure_size尺寸

    # lines = ["-", "--", ":","-.",]
    lines = ["-", "--", ":", ]
    markers = ["o", "^", "v", "s"]
    legend_setter = [{"linestyle": line, "marker": marker} for line in lines for marker in markers]
    linecycler = cycle(legend_setter)
    if idx == 0:
        plt.title('Violation Failed Test Group Result Analysis')
    elif idx == 1:
        plt.title('Constructing failed case Result Analysis ')
    else:
        raise ValueError("illegal idx para.")

    # 构造一个矩阵，转秩一下就行了?
    # name_list = [key for key in sorted(MR_info_dict.keys())]
    name_list = list(print_mr_name_list)
    name_list.reverse()
    origin_matrix_list = []
    for key in name_list:
        if key in MR_info_dict.keys():
            num_list = MR_info_dict[key]
        else:
            num_list = [0 for i in name_list]
        origin_matrix_list.append(num_list)
    mat = np.asmatrix(np.array(origin_matrix_list))
    mat = mat.T
    accumulate_mat = np.add.accumulate(mat, 0)
    print(mat)
    print(accumulate_mat)
    if np.asarray(accumulate_mat).shape == 0:
        accumulate_mat = [0]


    if GraphType == 0:
        for idx in range(mat.shape[0]):
            print(type(np.squeeze(np.asarray(mat[idx]))))
            if idx == 0:
                # np.squeeze(np.asarray(mat[idx]))
                plt.barh(name_list, np.squeeze(np.asarray(mat[idx])),  label=str(idx+1))
            else:
                plt.barh(name_list, np.squeeze(np.asarray(mat[idx])),
                         left=np.squeeze(np.asarray(accumulate_mat[idx - 1])), label=str(idx+1))
            # plt.bar(x_axis, num_list1, bottom=num_list, label='girl', tick_label=name_list, fc='r')
            # plt.legend(title = "Numbers of MR reveals the failed scenario")  # 显示图例
            plt.legend(title="Numbers of MR")  # 显示图例

    elif GraphType == 1:
        plt.barh(name_list,np.squeeze(np.asarray(accumulate_mat[-1])))
    else:
        raise ValueError("illegal GraphType para.")

    plt.tick_params(axis='y', labelsize=8)  # 设置x轴标签大小

    # plt.yticks(rotation=-30)  # 设置x轴标签旋转角度

    plt.xlabel('Violation Numbers')
    plt.ylabel('Numbers of failed test group')
    # 能不能定义个词汇， transmission caused 和 violation caused 的全集叫什么？？
    # not passed 我需要起名字！！！
    filename_png = os.path.splitext(filename)[0] + "_TotalCount_Type_" + str(GraphType) + ".eps"
    plt.savefig(filename_png)
    # plt.show()
    plt.cla()
    plt.clf()
    plt.close()

    #save data to json
    # b = dict( zip(name_list, [i.item() for i in np.squeeze(np.asarray(accumulate_mat[-1]))] ))
    # print(b)
    # output = json.dumps(b, indent=2)
    # json_filename = os.path.splitext(filename)[0] + "_violation_threshold_statistics.json"
    # # print(output)
    # with open(json_filename, "w") as f:
    #     f.writelines(output)

def ViolationThresholdFlows(base_path:str, statistics_type:str, threshold_type:str):
    from pathlib import Path
    import csv

    filename_png = "ThresholdFlows" +"-"+ statistics_type + "-" + threshold_type + ".eps"

    if statistics_type == "layer":
        file_name = "output_layer_compare0.csv"
    elif statistics_type == "mr":
        file_name = "output_MR_compare0.csv"
    else:
        raise ValueError("illegal statistics_type parameter.")

    if threshold_type != "Ratio" and  threshold_type != "Absolute":
        raise ValueError("illegal threshold_type parameter.")

    print("Begin create file: " + filename_png)
    x_values = []
    y_list = []
    os.chdir(base_path)
    for file_path in Path(base_path).rglob(file_name):
        #get value
        x = os.path.basename(file_path.parent).split('-')
        if x[1] == threshold_type:
            print("get the file:", file_path)
            x_values.append(float(x[0]))
            with open(file_path, newline='') as csvfile:
                reader = csv.DictReader(csvfile)
                data_dict = { str(row['name']):int(row['total']) for row in reader}
            y_list.append(data_dict)

    if len(y_list) == 0 or len(x_values) == 0:
        print("no graph generated, maybe no specific type of data(Ratio/Absolute)")
        return

    zip_dict =  dict(zip(x_values,y_list))


    plt.rcParams['savefig.dpi'] = 100  # 图片像素
    plt.rcParams['figure.dpi'] = 100  # 分辨率
    plt.rcParams['figure.figsize'] = (10, 8)  # 设置figure_size尺寸

    # lines = ["-", "--", ":","-.",]
    lines = ["-", "--", ":", ]
    markers = ["o", "^", "v", "s"]
    legend_setter = [{"linestyle": line, "marker": marker} for line in lines for marker in markers]
    linecycler = cycle(legend_setter)

    y_values={}
    for key in y_list[0].keys():
        y_values[key] = [zip_dict[zip_key].get(key, 0)  for zip_key in sorted(zip_dict.keys())]


    for key, y_value in y_values.items():
        plt.plot(sorted(zip_dict.keys()), y_value, **next(linecycler), label=key)
        # plt.xticks(sorted(zip_dict.keys()))

    plt.legend()  # 显示图例
    plt.xlabel( threshold_type + " value of threshold")
    plt.ylabel('Numbers of test group not passed')
    plt.savefig(filename_png)
    # plt.show()
    plt.cla()
    plt.clf()
    plt.close()

    print("File: " + filename_png +"complete")

def draw_mutiple_bar_plot_for_mr_scenes(agent_mr_pivot, file_path:str, mr_print_list: list, agent_print_list: list):

    # color_list = []
    # p = agent_mr_pivot.plot(kind='bar', stacked=True)
    # plt.show()

    df = agent_mr_pivot.reset_index().drop(["name_idx", "layer"], axis=1)
    df.set_index(['name'], inplace=True)
    df = df.T.reset_index()
    df.drop('level_0', inplace=True, axis=1)
    df.set_index(['agent'], inplace=True)
    df = df.T
    ax = (df * 100).plot(kind='bar', stacked=False) #percentage.
    plt.ylim((0,15))
    # plt.ylim((0,1000))


    plt.legend(loc='lower left', bbox_to_anchor=(1.04, 0), borderaxespad=0, title="agent",)  # 显示图例
    plt.xticks(rotation=30)
    plt.xlabel("Metamorphic Relations")
    plt.ylabel('Violation Rate')
    # plt.xlabel("Baseline Algorithm")
    # plt.ylabel('Failed Rate')
    ax.yaxis.set_major_formatter(mtick.PercentFormatter())


    # plt.show()
    plt.savefig(file_path, bbox_inches='tight')
    plt.cla()
    plt.clf()
    plt.close()

def draw_mutiple_bar_plot_for_layer_scenes(agent_mr_pivot, file_path:str, layer_print_list: list, agent_print_list: list):

    df = agent_mr_pivot.reset_index().drop(["layer_idx"], axis=1)
    df.set_index(['layer'], inplace=True)
    df = df.T.reset_index()
    df.drop('level_0', inplace=True, axis=1)
    df.set_index(['agent'], inplace=True)
    df = df.T
    ax = (df * 100).plot(kind='bar', stacked=False)
    # plt.ylim((0,1000))
    plt.ylim((0, 15))


    plt.legend(loc='lower left', bbox_to_anchor=(1.04, 0), borderaxespad=0, title="agent",)  # 显示图例
    plt.xticks(rotation=30)
    plt.xlabel("Layers")
    plt.ylabel('Violation Rate')
    ax.yaxis.set_major_formatter(mtick.PercentFormatter())

    # plt.show()
    plt.savefig(file_path, bbox_inches='tight')
    plt.cla()
    plt.clf()
    plt.close()