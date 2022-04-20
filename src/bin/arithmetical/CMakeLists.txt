cmake_minimum_required(VERSION 3.3)

project(exam)

file(GLOB SRC src/*.cpp)
include_directories(include)

add_subdirectory(tests/unit)

add_executable(arithmetical ${SRC})