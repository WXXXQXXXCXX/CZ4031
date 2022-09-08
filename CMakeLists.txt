cmake_minimum_required(VERSION 3.23)
project(CZ4031)

set(CMAKE_CXX_STANDARD 14)

include_directories(storage)

add_executable(CZ4031
        storage/blks.cpp
        storage/blks.h
        storage/mem_test.cpp)
