<pre>
███████╗███████╗████████╗████████╗██╗███╗   ██╗ ██████╗     ███████╗ ██████╗ ██████╗     
██╔════╝██╔════╝╚══██╔══╝╚══██╔══╝██║████╗  ██║██╔════╝     ██╔════╝██╔═══██╗██╔══██╗    
███████╗█████╗     ██║      ██║   ██║██╔██╗ ██║██║  ███╗    █████╗  ██║   ██║██████╔╝    
╚════██║██╔══╝     ██║      ██║   ██║██║╚██╗██║██║   ██║    ██╔══╝  ██║   ██║██╔══██╗    
███████║███████╗   ██║      ██║   ██║██║ ╚████║╚██████╔╝    ██║     ╚██████╔╝██║  ██║    
╚══════╝╚══════╝   ╚═╝      ╚═╝   ╚═╝╚═╝  ╚═══╝ ╚═════╝     ╚═╝      ╚═════╝ ╚═╝  ╚═╝    
                                                                                         
 ██████╗ ██████╗ ██████╗ ███████╗    ██████╗ ██╗██████╗ ████████╗██████╗                 
██╔════╝██╔═══██╗██╔══██╗██╔════╝    ██╔══██╗██║██╔══██╗╚══██╔══╝╚════██╗                
██║     ██║   ██║██║  ██║█████╗      ██████╔╝██║██████╔╝   ██║    █████╔╝                
██║     ██║   ██║██║  ██║██╔══╝      ██╔═══╝ ██║██╔══██╗   ██║   ██╔═══╝                 
╚██████╗╚██████╔╝██████╔╝███████╗    ██║     ██║██████╔╝   ██║   ███████╗                
 ╚═════╝ ╚═════╝ ╚═════╝ ╚══════╝    ╚═╝     ╚═╝╚═════╝    ╚═╝   ╚══════╝                


</pre>

---

Yang Liu, 
2022.8.4,

Email: yangliu2@bjtu.edu.cn

---

## Refactoring

## 1) Delete the Marco ***\_MAPDIR_***
In order to support the Metamorphic Relations(MRs) of the environment level, we have to create new maps based on the MRs. So, the absolute address of the map is required.

Since the code specific the Macro in the cmakefile of ***/pibt2/CMakeLists.txt*** and the makefiles of ***/visualizer/Makefile***, We have to delete the Marco ***\_MAPDIR_***

## 2) Change the Path Value ***OF_ROOT***
Since the code specific the value of ***OF_ROOT*** in the makefile of ***/visualizer/Makefile***, I have to set it up to fit my environment. I have installed the openFrameworks and set my **OF_ROOT*** in my ***.zshrc***. So I delete the definition of ***OF_ROOT*** in the makefile of ***/visualizer/Makefile***.


