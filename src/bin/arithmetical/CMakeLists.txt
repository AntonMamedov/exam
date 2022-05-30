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
include(cmake/common.cmake)

# Proto file
get_filename_component(proto "../../../protos/arithmetical.proto" ABSOLUTE)
get_filename_component(proto_path "${proto}" PATH)

set(PROTO_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Generated sources
set(proto_srcs "${PROTO_SRC_DIR}/arithmetical.pb.cc")
set(proto_hdrs "${PROTO_SRC_DIR}/arithmetical.pb.h")
set(grpc_srcs "${PROTO_SRC_DIR}/arithmetical.grpc.pb.cc")
set(grpc_hdrs "${PROTO_SRC_DIR}/arithmetical.grpc.pb.h")
add_custom_command(
        OUTPUT "${proto_srcs}" "${proto_hdrs}" "${grpc_srcs}" "${grpc_hdrs}"
        COMMAND ${_PROTOBUF_PROTOC}
        ARGS --grpc_out "${PROTO_SRC_DIR}"
        --cpp_out "${PROTO_SRC_DIR}"
        -I "${proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${proto}"
        DEPENDS "${proto}")


include_directories("${PROTO_SRC_DIR}")

add_library(grpc_proto
        ${grpc_srcs}
        ${grpc_hdrs}
        ${proto_srcs}
        ${proto_hdrs})
target_link_libraries(grpc_proto
        ${_REFLECTION}
        ${_GRPC_GRPCPP}
        ${_PROTOBUF_LIBPROTOBUF})

file(GLOB PROTO_SRC ${PROTO_SRC_DIR}/*.cc)
add_executable(arithmetical src/main.cpp src/service.cpp src/engine.cpp src/bitset.cpp ${PROTO_SRC})
target_link_libraries(arithmetical
        grpc_proto
        ${_REFLECTION}
        ${_GRPC_GRPCPP}
        ${_PROTOBUF_LIBPROTOBUF})
