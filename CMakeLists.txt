cmake_minimum_required(VERSION 3.31.6)
project(Repetitor)

set(CMAKE_CXX_STANDARD 23)

add_executable(Repetitor main.cpp
        preprocessor.h
        preprocessor.cpp
        splitter.cpp
        splitter.h
        source_module.cpp
        source_module.h)

enable_testing()

add_test(
        NAME Repetitor
        COMMAND Repetitor
)