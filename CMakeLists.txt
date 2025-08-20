cmake_minimum_required(VERSION 3.31.6)
project(RepetitorProject)

set(CMAKE_CXX_STANDARD 23)

add_custom_target(CheckCPP ALL bash ${CMAKE_CURRENT_SOURCE_DIR}/check_cpp.sh)

add_executable(Repetitor main.cpp
        preprocessor.h
        preprocessor.cpp
        splitter.cpp
        splitter.h
        source_module.cpp
        source_module.h)

add_dependencies(Repetitor CheckCPP)

enable_testing()

add_test(
        NAME Repetitor
        COMMAND Repetitor
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/run
)