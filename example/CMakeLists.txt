cmake_minimum_required(VERSION 3.10)

project(lotus C CXX)

add_definitions(-std=c++14)

set(CXX_FLAGS
 -g
 -DCHECK_PTHREAD_RETURN_VALUE
 -D_FILE_OFFSET_BITS=64
 -Wall
 -Wextra
 -Werror
 -march=native
 -std=c++14
 -rdynamic
 )

include_directories(../lotus)
add_subdirectory(../lotus lotus_lib_dir)

add_executable(rpc_server ./rpc_server.cpp)
target_link_libraries(rpc_server lotus pthread)

add_executable(rpc_client ./rpc_client.cpp)
target_link_libraries(rpc_client lotus pthread)

add_executable(http_server ./http_server.cpp)
target_link_libraries(http_server lotus pthread)
