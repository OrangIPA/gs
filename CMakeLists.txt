cmake_minimum_required(VERSION 3.20.0)

project(gs VERSION 0.0.1)

set(CMAKE_C_STANDARD 17)

file(GLOB_RECURSE SRC_FILES src/*.c)
add_executable(gs ${SRC_FILES})

target_include_directories(gs PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)