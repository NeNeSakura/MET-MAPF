cmake_minimum_required(VERSION 3.15)

# set the project name and version
project(MT-Framework)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
# set(CMAKE_CXX_STANDARD_REQUIRED True)

#FIND_PACKAGE(concurrentqueue REQUIRED)

# set boost
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost REQUIRED COMPONENTS regex filesystem date_time thread chrono)
# set boost complete

# multi-threads required
FIND_PACKAGE ( Threads REQUIRED )


# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_SRCS 变量
file(GLOB_RECURSE CPP_FILES ./*.cpp ./*.cc)
file(GLOB_RECURSE H_FILES ./*.h ./*.hpp)

set(3rdParty_PATH "../3rdParty/")
file(GLOB_RECURSE 3rd_FILES
        ${3rdParty_PATH}*.h
        ${3rdParty_PATH}*.hpp
        ${3rdParty_PATH}*.cpp
        ${3rdParty_PATH}*.cc)

set(SOURCE_FILES ${CPP_FILES} ${H_FILES} ${3rd_FILES})

#include_directories("${PROJECT_BINARY_DIR}")

MESSAGE( STATUS "this var key = ${concurrentqueue_INCLUDE_DIRS}.")

#add _SUT_VERSION_
add_definitions(-D_SUT_VERSION_="2.0")


include_directories(
        ${3rdParty_PATH}
        ${Boost_INCLUDE_DIRS}
)

add_library(
        MT-Framework SHARED
        ${SOURCE_FILES})

target_link_libraries(
        MT-Framework
        MemPool
        ${CMAKE_THREAD_LIBS_INIT}
        ${Boost_LIBRARIES}
        )
