cmake_minimum_required(VERSION 3.20)
project(BTreeProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(btree
        main.cpp
        btree.cpp
        utils.cpp
        range.cpp
)