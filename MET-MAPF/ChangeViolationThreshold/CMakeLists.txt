cmake_minimum_required(VERSION 3.15)
project(ChangeViolationThreshold)

set(CMAKE_CXX_STANDARD 14)


# set boost
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost REQUIRED COMPONENTS regex filesystem)
# set boost complete

#设定Cmake 搜索路径
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/modules")


# 查找当前目录下的所有源文件
# 并将名称保存到 SOURCE_FILES 变量
file(GLOB_RECURSE CPP_FILES ./*.cpp ./*.cc)
file(GLOB_RECURSE H_FILES ./*.h ./*.hpp)
set(SOURCE_FILES ${CPP_FILES} ${H_FILES})


#include_directories("${PROJECT_BINARY_DIR}")
set(TestingFramework_PATH "../MT-Framework/")
set(3rdParty_PATH "../3rdParty/")
include_directories (
        "${TestingFramework_PATH}"
        "${3rdParty_PATH}"
        "${Boost_INCLUDE_DIRS}"
)

#find_package(Cairo REQUIRED)

add_executable(ChangeViolationThreshold ${SOURCE_FILES})
#target_include_directories(HelloCairo PRIVATE ${Cairo_INCLUDE_DIRS})

target_link_libraries(
        ChangeViolationThreshold
        PRIVATE
        MT-Framework
        ${Boost_LIBRARIES}
)



