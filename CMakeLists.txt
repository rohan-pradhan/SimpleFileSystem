cmake_minimum_required(VERSION 3.6)
project(Assignment_3)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    disk_emu.c
    disk_emu.h
    fuse_wrappers.c
    sfs_api.c
    sfs_api.h
    sfs_test1.c
    sfs_test2.c
    tests.c
    tests.h INode.h SuperBlock.h OpenFile.h DataSize.h Directory.h FreeBitMap.h)

add_executable(Assignment_3 ${SOURCE_FILES})