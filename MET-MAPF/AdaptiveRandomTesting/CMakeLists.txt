cmake_minimum_required(VERSION 3.15)
project(AdaptiveRandomTesting)

set(CMAKE_CXX_STANDARD 14)

#设定Cmake 搜索路径
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/modules")


# 查找当前目录下的所有源文件
# 并将名称保存到 SOURCE_FILES 变量
file(GLOB_RECURSE CPP_FILES ./*.cpp ./*.cc)
file(GLOB_RECURSE H_FILES ./*.h ./*.hpp)
set(SOURCE_FILES ${CPP_FILES} ${H_FILES})


#include_directories("${PROJECT_BINARY_DIR}")
set(MT_TestingFramework_PATH "../MT-Framework/")
#set(ART_TestingFramework_PATH "../ART-Framework/")
set(3rdParty_PATH "../3rdParty/")
include_directories (
        ${MT_TestingFramework_PATH}
        ${ART_TestingFramework_PATH}
        ${3rdParty_PATH}
        )

#find_package(Cairo REQUIRED)

add_executable(AdaptiveRandomTesting ${SOURCE_FILES})
#target_include_directories(HelloCairo PRIVATE ${Cairo_INCLUDE_DIRS})
target_link_libraries(
        AdaptiveRandomTesting
        PRIVATE
        MT-Framework
#        ART-Framework
)



