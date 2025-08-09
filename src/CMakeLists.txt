cmake_minimum_required(VERSION 2.6)
project(ThreadPool)
set(CMAKE_CXX_STANDARD 17)

set(LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

aux_source_directory(. SRC_LIST)

add_library(ThreadPool SHARED ${SRC_LIST})

# 可选：打印路径用于验证
message(STATUS "动态库将生成到: ${LIBRARY_OUTPUT_DIRECTORY}")

set_target_properties(ThreadPool PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")