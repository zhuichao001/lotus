cmake_minimum_required(VERSION 3.10)

project(lotus C CXX)

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

aux_source_directory(. DIR_LIB_LOTUS_SRCS)
aux_source_directory(./protocol DIR_LIB_LOTUS_PROTOCOL_SRCS)
add_library(lotus STATIC ${DIR_LIB_LOTUS_SRCS} ${DIR_LIB_LOTUS_PROTOCOL_SRCS})
