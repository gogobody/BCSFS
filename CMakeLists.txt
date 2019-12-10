cmake_minimum_required(VERSION 3.10)
project(BCSFS)

set(CMAKE_CXX_STANDARD 11)

find_package(PkgConfig REQUIRED)
pkg_search_module(FUSE3 REQUIRED fuse3)
IF("${FUSE3_FOUND}" STREQUAL "1")
    set(INCLUDE_DIRS
            ${FUSE3_INCLUDE_DIRS})
ELSE("${FUSE3_FOUND}" STREQUAL "1")
    message("fuse3 pkg config not found!")
ENDIF("${FUSE3_FOUND}" STREQUAL "1")

set(LDFLAGS `pkg-config fuse3 --libs`)
#set(CFLAGS -Wall --std=c++14 `pkg-config fuse3 --cflags` -I..)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall --std=c++14 `pkg-config fuse3 --cflags` -I..")
set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror pkg-config fuse3 --cflags")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DFUSE_USE_VERSION=32")


find_package(FUSE3 REQUIRED)
include_directories(${FUSE_INCLUDE_DIR})

add_executable(BCSFS main.cpp BCSFS.cpp BCSFS.h)
target_link_libraries(main ${FUSE_LIBRARIES})