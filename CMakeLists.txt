cmake_minimum_required(VERSION 3.15)
project(PerformanceAndSecurity CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

file(GLOB SOURCES "src/*.cpp")

foreach(source_file ${SOURCES})
    get_filename_component(exe_name ${source_file} NAME_WE)
    add_executable(${exe_name} ${source_file})
endforeach()
