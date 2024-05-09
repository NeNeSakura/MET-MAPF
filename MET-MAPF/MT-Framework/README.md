# Tree Structure

MT
- single-testing(name=timestamp)/
  - init-param.txt(仿真任务参数)
  - mt-param (json,MT 仿真参数)
    ```
    testing_idx = timestamp
    metamorphic relation list
    {
        relation type name
        relation id //可能有多个同type的
        relation param(json字符串？？)
        bool testing criteria(testing result list);
    }
    execution times //每个场景的次数？
    map list //场景列表
    ```

  - scenarios/  

    - map/
      ```
      *.map
      //将所需的 map copy到这里，并将做了各种变换操作的场景也放在这里。
      ```
    - task/
      - task-MR-NAME/ 
    
        - \# iteration/ (different random seed)
      
          ```
          taskinfo-*.json
          //所有的仿真起点终点的输入
          //从 init-param 构造出来的临时 param 参数
          ```
    - tmp/(临时生成的)
        
      ```
      考虑到每个场景在生成时，task获取的方式是由运行完仿真程序得到的task -*.json生成的。而这个初始task*.json文件很容易跟其他文件混在一起，所以，在该目录下获取json后，通过MR将衍生的测试用例扔到task。当所有用例生成完后，清空tmp目录。
      ```
         

运行流程：

1.准备好 init-param.txt 确定各个场景的统一仿真参数
（关于目标数量这个决定用0～0.5的实数值来代表比例。毕竟，起点和终点不能铺满整个场景的）

2.准备好 MR 参数文件。
依照参数文件信息，将每个场景的，每一种 MR 的，循环次数的， taskinfo 文件准备好。内存保留taskinfo的json字符串，同样也保存到硬盘上。

3.依照内存中的链表去运行每个仿真任务
运行时，以taskinfo 中的param 信息为准（随机数种子一致）。由taskinfo生成param的txt（或者改造算法输入）
将输出文件移动到合适的目录中
移动到：
log/MT/single-testing/MR-Name/\#iteration/下

4.数据处理，依据内存中 taskinfo 的 relation
去读取相应MR_name 中分析各个 \#iteration的数据结果。
将统计结果以 json形式保存
最后在python中统计，绘图