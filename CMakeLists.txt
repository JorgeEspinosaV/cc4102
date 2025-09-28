cmake_minimum_required(VERSION 4.0)
project(cc4102)

set(CMAKE_CXX_STANDARD 14)

include_directories(t1/data)
include_directories(t1/models)

add_executable(cc4102
        t1/models/btree.cpp
        t1/models/node.cpp
        t1/models/node.h
        t1/tarea-1.pdf)
