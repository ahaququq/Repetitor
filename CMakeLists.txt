cmake_minimum_required(VERSION 4.0)
project(Repetitor)

set(CMAKE_CXX_STANDARD 26)

add_executable(Repetitor main.cpp
        preprocessor.h
        preprocessor.cpp
        splitter.cpp
        splitter.h
        source_module.cpp
        source_module.h)
