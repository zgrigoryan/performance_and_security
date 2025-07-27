cmake_minimum_required(VERSION 3.15)
project(PerformanceAndSecurity CXX)

# ── use C++14 (needed for digit separators)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# collect all sources
file(GLOB ALL_SRC "src/*.cpp")

# ---------- Windows portability tweaks ----------
if (WIN32)
    #   • mmap/mprotect/unistd      → skip for Windows
    #   • noinline attribute clash  → keep; MSVC just ignores GCC attribute
    list(REMOVE_ITEM ALL_SRC
         "${CMAKE_SOURCE_DIR}/src/security_defenses.cpp")
endif()

# build every remaining .cpp into its own executable
foreach(src ${ALL_SRC})
    get_filename_component(exe ${src} NAME_WE)
    add_executable(${exe} ${src})
endforeach()

# Optional: make Release the default single‑config build
if (NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()
