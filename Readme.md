<pre>
███╗   ███╗███████╗████████╗   ███╗   ███╗ █████╗ ██████╗ ███████╗                                               
████╗ ████║██╔════╝╚══██╔══╝   ████╗ ████║██╔══██╗██╔══██╗██╔════╝                                              
██╔████╔██║█████╗     ██║      ██╔████╔██║███████║██████╔╝█████╗                                              
██║╚██╔╝██║██╔══╝     ██║      ██║╚██╔╝██║██╔══██║██╔═══╝ ██╔══╝                                              
██║ ╚═╝ ██║███████╗   ██║      ██║ ╚═╝ ██║██║  ██║██║     ██║                                              
╚═╝     ╚═╝╚══════╝   ╚═╝      ╚═╝     ╚═╝╚═╝  ╚═╝╚═╝     ╚═╝                                            
</pre>                                                                                  
                                                                                
---

23.08.2023

---

# 1. What is this?

This is the experiment code of the paper 
"MET-MAPF: A Metamorphic Testing Approach for Multi-Agent Path Finding Algorithms".
The code has been tested through Ubuntu 20.04 and Manjaro 21.3.1 . 


# 2. What does this code contain?

## **Through our code, we can implement**

- A framework of Metamorphic Testing (MT) for the MAPF system
- Random Testing as the baseline technique 1
- Adaptive Random Testing as the baseline technique 2

## **The Solution contains the following *projects*:**

- MET-MAPF 
   - MT-Framework: The MT framework.
   - MetamorphicTesting: The application of MT
   - RandomTesting: The application of Random Testing
   - ART: The application of Adaptive Random Testing
   - DataAnalysisScript: The python scripts for data collection and analysis.
   - 3rdParty: Some open-source libraries supporting the MT-Framework (can be cloned from github).
- pibt2: The System Under Test (SUT), which is provided by a recent work:
```

@article{okumura2022priority,
title = {Priority Inheritance with Backtracking for Iterative Multi-agent Path Finding},
journal = {Artificial Intelligence},
pages = {103752},
year = {2022},
issn = {0004-3702},
doi = {https://doi.org/10.1016/j.artint.2022.103752},
author = {Keisuke Okumura and Manao Machida and Xavier Défago and Yasumasa Tamura},
}
```
Note that, their code, realizing the MAPF system and benchmark PIBT algorithms, is open-source and can be achieved at
https://kei18.github.io/pibt2/

# 3. Dependencies 

To execute our code, we should first install *cmake* and *boost* for our three-layer framework.
Also, running the benchmark MAPF system requires some additional dependencies from *openFrameworks*.


## Requirements for Metamorphic Testing Framework:
- Cmake, at least VERSION 3.15
    - ubuntu
        ```shell
        sudo apt install cmake
        ```
    - manjaro
        ```shell
        sudo pacman -Syu cmake
        ```

- Boost   
    - ubuntu
    ```shell
    sudo apt install libboost-all-dev
    ```

    - manjaro
    ```shell
    pacman -Sy boost boost-libs
    ```
   
**Note:** 
  ***Boost*** should be ***full version***, as some components such as ***regex***, ***filesystem*** and ***date_time*** are required.
 

## Requirements for the MAPF System Under Test:

-  ***Boost*** and ***Eigen3***, if only command line interface(CLI) is required.
    - ubuntu 
    ```shell
    sudo apt install libboost-all-dev libeigen3-dev
    ``` 

    - manjaro
    ```shell
    pacman -Sy boost boost-libs eigen
    ``` 

- ***openframework***, if GUI is expected.
  
  Since the GUI works based on the CLI, ***Boost*** and ***Eigen3*** should be installed first. Then installs ***openframework***.

  Please follow the instructions of *openFrameworks* at https://openframeworks.cc/setup/linux-install/. Basically, two steps are required.
	
  - Install openFramework dependencies

    ```shell
    cd OF/scripts/linux/ubuntu
    sudo ./install_dependencies.sh
    ```

  - compile the openFrameworks
    ```shell
    cd OF/scripts/linux
    ./compileOF.sh
    ```
**Note:** 
The cloned pibt2 project hold the ***openFramework*** as submodule, so ***OF*** is the address of the submodule folder.

# 4. Compile 
The code is compiled by clang++ 10.0.0

- Build Metamorphic Testing Framework (MET-MAPF) 
   
    Just use cmake to get the makefile and make it like this
    ```shell
    cd MET-MAPF/
    mkdir build 
    cd build
    cmake ..
    make 
    ```

- Build the simulator of Iterative Multi-agent Path Finding (i.e., the benchmark MAPF system)
    
    - Build CLI
    ```shell
    cd pibt2
    mkdir build && cd build
    cmake ..
    make 
    ```  

    - Build GUI
    ```shell
    cd visualizer
    make build
    ```


# 5. Execution

There are 3 projects rely on MT-Framework:
- MetamorphicTesting
- RandomTesting
- AdaptiveRandomTesting

After building successfully, go to the build folder and execute the generated binary files, including "RandomTesting", "AdaptiveRandomTesting" and "MetamorphicTesting".

```shell
cd build/MetamorphicTesting
./MetamorphicTesting
```
```shell
cd build/RandomTesting
./RandomTesting
```
```shell
cd build/AdaptiveRandomTesting
./AdaptiveRandomTesting
```


When the execution of these code complete, data analysis script is participated. The *"MetamorphicTesting"*, *"RandomTesting"* and *"AdaptiveRandomTesting"* code will automatically call this script.

Just in case, if something wrong happened, just call it by command line. Call the ReadFile.py to generate all statistics data and figures. 

```shell
python3 ReadFile.py -i "your output data path"
```


If you want to change the violation criteria, project ChangeViolationThreshold may help you update the statistics information. Set the expected violation threshold type and value and build the project. (It's very short and clear to change.)

By the way, don't forget to specify the root.
```shell
    //change the path to the selected one
    auto _base = path("/home/hiccup/Appendix/ZXY-Tmp/MetamorphicTestingData/20210206T222338-Result/");
```

Finally, just execute the code in command line.
```shell
cd build/ChangeViolationThreshold
./ChangeViolationThreshold
```
---

Note that, if you have installed **tmux** and **tmuxinator**, you can use **Experiment.sh** or **FastCall.sh** in the folder **TmuxinatorScript** to execute the whole experiment with one command. There are 3 files in the folder **TmuxinatorScript**:
- .tmuxinator.yml
- ExperimentInit.sh
- FastCall.sh

**.tmuxinator.yml** records the setting of tmuxinator, which could create and manage tmux sessions easily. In this setting, we will execute each experiment with 10 parallel processes, and add the results up to get the statistics. This setting implies that each process executes 10% testing work of the experiment. The detailed information could visit: https://github.com/tmuxinator/tmuxinator

**ExperimentInit.sh** calls tmuxinator with predefined settings to execute the experiment. For example:
```bash
 # Usage: ./ExperimentInit.sh <workspace_path> <tested_program> <tmux session name>
 ./ExperimentInit.sh ../build/ARTTesting ./ARTTesting ART
```

**FastCall.sh** calls ExperimentInit.sh with binary files "RandomTesting", "AdaptiveRandomTesting" and "MetamorphicTesting".
```bash
 # Usage: ./FashCall.sh <workspace_path>
 ./FashCall.sh ../build
```



# 6. How to config the Metamorphic Testing Framework?

Generally speaking, experiment setting are listed in these 2 projects, including the testing algorithm, selected maps, selected agent numbers, and other configures of the simulation. Here is an example:

```c++
    std::vector<std::string> algorithms = {"PIBT",  "PIBT_PLUS", "HCA"};

    std::vector<double> agent_num = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};

    std::vector<std::string> maps = {
            "Berlin_1_256.map",
            "Boston_0_256.map",
            "Paris_1_256.map",
            "Shanghai_0_256.map",
            "Milan_0_256.map",
            "brc202d.map",
            "den312d.map",
            "den520d.map",
            "ht_chantry.map",
            "ht_mansion_n.map",
            "lak303d.map",
            "ost003d.map",
            "warehouse-10-20-10-2-1.map",
            "warehouse-10-20-10-2-2.map",
            "maze-32-32-4.map",
            "maze-128-128-10.map",
            "random-32-32-10.map",
            "random-64-64-10.map",
            "room-64-64-8.map",
            "room-64-64-16.map",
    };

```

Here, MetamorphicTesting and RandomTesting construct the instance of class MAPF_TestingFile to prepare input file for MT-Framework.

```c++
class MAPF_TestingFile {
  MT_HyperPara para;  //Hyper Parameters
  size_t repeat_unit; //test group size
  size_t repeat_case; //repeat times for each test group(default 0)
  
  string data_path_root;  //path to save output data of MAPF system
  string case_path_root;  //path to get the map data of MAPF system
  string tmp_file_path_root;  //path to save tmp files
  string test_program_root;   //path to get the testing program
  
  size_t idx; //epoch index
  size_t total_num; //total epoch size;
  
  std::vector<std::string> maps; //benchmark maps for testing
  
  string GUI_path;    //test program path, GUI version
  string CLI_path;    //test program path, CLI version
  string log_config_filename; //log config file
  bool save_log = false;  //save MT-framework log?
  bool is_CLI = true;     //select CLI / GUI, true for CLI
  
  double validation_threshold;     // init validation threshold, Could be updated by Project ChangeViolationThreshold
  ViolationCriteria::ThresholdTypeEnum validation_threshold_type; // init validation type
}
```

As for the definition of the metamorphic relation, check MR_ParameterGenerator.cpp to get detail information. If you want to run this code without any new MR involved, just leave this part.

Here is an example of MR definition:
```c++
nlohmann::json j_para = OperMirrorParam{
    OperMirrorParam::horizon
};
TestingFramework::MR_Parameter mirror_1{
    boost::uuids::to_string(boost::uuids::random_generator()()),    //uuid
    "mr_2-p1",          //MR name
    "",                 //MR description
    "MT-Framework.so",  //File source description
    "Mirror",           //MR type
    random_list,        //random list
    j_para.dump(),      //parameter for MR
};
```

It is worth mentioning that the folders should be legal path, including:

- data_path_root;  //path to save output data of MAPF system
- case_path_root;  //path to get the map data of MAPF system
- tmp_file_path_root;  //path to save tmp files
- test_program_root;  //path to get the testing program
- GUI_path;  //test program path, GUI version 
- CLI_path;  //test program path, CLI version



# 7. Related to the output 

The following files in the output folder are important:

- output-layer9.csv： the violation rate of all the MRs under different settings of agents numbers. 
- output-layer11.csv: the violation rate of all the MRs in each map (i.e., environment)
- Two_MR_comparison_ratio_0.xlsx: The uniqueness of each MR pairs (i.e., Fig. 3 in the paper)
- 1vsOther_MR_Comparison: The uniqueness between each MR and other MRs (i.e., Fig. 4 in the paper)
- output.csv and output2.csv: The detailed violation/failure rate. If we run RT and ART, we can get the failure rate from the column "legal_case_percent"

<!-- 
    **Execute makefile constructions and make it**

    - complie the PIBT algorithm 
        ```
        make c
        ```

    - fathermore if we want to implement with openFrameworks (i.e., GUI mode)

        First, add OF_ROOT to the system environment, such as .bashrc or .zshrc:
        ```
        export OF_ROOT={your openFrameworks directory}
        ```
        Then use the following commands:
        ```
        cd MET-MAPF/PIBT
        make of
        ```
        Note that for some OS we may need to install Eigen

        ```
        sudo apt install libeigen3-dev
        ``` 
    alright? -->
