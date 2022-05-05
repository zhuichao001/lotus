cmake_minimum_required(VERSION 3.20)

project(lotus C CXX)

add_definitions(-std=gnu++11)

set(CXX_FLAGS
 -g
 -DCHECK_PTHREAD_RETURN_VALUE
 -D_FILE_OFFSET_BITS=64
 -Wall
 -Wextra
 -Werror
 -Wconversion
 -Wno-unused-parameter
 -Wold-style-cast
 -Woverloaded-virtual
 -Wpointer-arith
 -Wshadow
 -Wwrite-strings
 -march=native
 -std=c++14
 -rdynamic
 )

include_directories(../lotus)
add_subdirectory(../lotus lotus_lib_dir)

add_executable(climain ./climain.cpp)
target_link_libraries(climain lotus pthread)

add_executable(svrmain ./svrmain.cpp)
target_link_libraries(svrmain lotus pthread)
