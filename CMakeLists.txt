cmake_minimum_required(VERSION 3.31)
project(vsprofile)

set(CMAKE_CXX_STANDARD 20)

add_executable(vsprofile main.cpp
        Config.cpp
        Core.cpp
)
