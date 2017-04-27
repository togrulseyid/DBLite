#cmake_minimum_required(VERSION 3.7)
cmake_minimum_required(VERSION 3.5.2)
project(DBLite)

set(PROJECT_BRIEF "Database Systems on Modern CPU Architectures")
set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
include(version/version.cmake)

# include directory for header files
include_directories("include/")

# Flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall")
#set(CMAKE_CXX_STANDARD 14)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE DEBUG)

# Source files
set(SOURCE_FILES
        src/main.cpp
        src/operations.cpp
        src/sort.cpp
        ${version_file}
        include/dblite.h
        include/operations.hpp
#        include/plog/Log.h
        include/sort.h)

# Executable
add_executable(DBLite ${SOURCE_FILES})

target_include_directories(DBLite PUBLIC include/)

# Test
add_custom_target(stest
        ../test/test.sh
        )

# doxygen build
set(DOXYGEN_TARGETS DBLite)
include(doc/docs.cmake)