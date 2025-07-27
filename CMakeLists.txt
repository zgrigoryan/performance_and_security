cmake_minimum_required(VERSION 3.15)
project(PerformanceAndSecurity CXX)
if (MSVC)
    add_compile_options(/wd5030)   # “attribute is not recognized”
endif()


# We need C++14 for digit separators
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# -------------------------------------------------------------------------
# Collect platform‑appropriate sources
# -------------------------------------------------------------------------
file(GLOB SRC_ALL "src/*.cpp")

set(SRC_WINDOWS_EXCLUDE
    "${CMAKE_SOURCE_DIR}/src/security_defenses.cpp"
    "${CMAKE_SOURCE_DIR}/src/return_oriented_prog.cpp")   # <unistd.h>

if (WIN32)
    list(REMOVE_ITEM SRC_ALL ${SRC_WINDOWS_EXCLUDE})
endif()

# -------------------------------------------------------------------------
# Build every .cpp -> standalone executable (same base name)
# -------------------------------------------------------------------------
foreach(src ${SRC_ALL})
    get_filename_component(exe ${src} NAME_WE)
    add_executable(${exe} ${src})
endforeach()
