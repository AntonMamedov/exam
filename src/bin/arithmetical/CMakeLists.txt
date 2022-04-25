cmake_minimum_required(VERSION 3.19)

project(exam)

set(CMAKE_CXX_STANDARD 20)

find_package (Boost REQUIRED)

if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    message("Boost founded\n")
else()
    message("Boost is not found\n")
endif()

file(GLOB SRC src/*.cpp)
include_directories(include)

add_subdirectory(tests/unit)

add_executable(arithmetical ${SRC})