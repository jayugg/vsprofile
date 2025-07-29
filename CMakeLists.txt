cmake_minimum_required(VERSION 3.31)
project(vsprofile)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(vsprofile
        Core/main.cpp
        Core/Config.cpp
        Core/Core.cpp
        Utils/FileUtils.cpp
)