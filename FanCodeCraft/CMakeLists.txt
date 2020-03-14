cmake_minimum_required(VERSION 3.5)
project(multiThread)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -O0 -ggdb")
set(CMAKE_CXX_STANDARD 11)
include_directories(${PROJECT_SOURCE_DIR}/tools/timer/inc)
include_directories(${PROJECT_SOURCE_DIR}/tools/timer/src)
include_directories(${PROJECT_SOURCE_DIR}/tools/threads)

AUX_SOURCE_DIRECTORY(${PROJECT_SOURCE_DIR}/test  DIR_SRCS)

ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/tools/threads)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/tools/timer)

add_executable(example ${DIR_SRCS})
target_link_libraries(example pthread)
target_link_libraries(example timer)