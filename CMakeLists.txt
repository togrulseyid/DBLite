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
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall  -g -O1 -pthread ")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -march=native -std=c++14 -g -O3 -pthread ")
#set(CMAKE_CXX_STANDARD 14)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE DEBUG)

# Source files
set(ESORT_FILES
        test/esorttest.cpp
        src/operations.cpp
        src/e_sort/sort.cpp
        ${version_file}
        include/dblite.h
        include/operations.hpp
        #        include/plog/Log.h
        include/e_sort/sort.h)


# Source files
set(BUFFER_FILES
        test/buffertest.cpp
        src/operations.cpp
        ${version_file}
        src/buffer/buffermanager.cpp
        src/buffer/bufferframe.cpp
        src/slottedpages/spsegment.cpp
        src/slottedpages/slottedpages.cpp
        src/slottedpages/Record.cpp
        src/slottedpages/schemasegment.cpp
        src/slottedpages/segment.cpp)

# Source files
set(SLOTTED_PAGE_FILES
        test/slottedTest.cpp
        src/operations.cpp
        ${version_file}
        src/buffer/buffermanager.cpp
        src/buffer/bufferframe.cpp
        src/slottedpages/spsegment.cpp
        src/slottedpages/slottedpages.cpp
        src/slottedpages/Record.cpp
        src/slottedpages/segment.cpp
        src/slottedpages/schemasegment.cpp)

set(SCHEMA_FILES
        test/schematest.cpp
        src/operations.cpp
        ${version_file}
        src/buffer/buffermanager.cpp
        src/buffer/bufferframe.cpp
        src/slottedpages/spsegment.cpp
        src/slottedpages/slottedpages.cpp
        src/slottedpages/Record.cpp
        src/slottedpages/segment.cpp
        src/slottedpages/schemasegment.cpp
        include/slottedpages/schema/Parser.cpp
        include/slottedpages/schema/Schema.cpp)

set(BTREE_FILES
        ${version_file}
        src/buffer/buffermanager.cpp
        src/buffer/bufferframe.cpp
        src/slottedpages/segment.cpp
        src/btree/btree.cpp
        test/btreetest.cpp)

set(OPERATORS_FILES
        ${version_file}
        src/operators/operators.cpp
        src/operators/register.cpp
        src/buffer/buffermanager.cpp
        src/buffer/bufferframe.cpp
        src/slottedpages/segment.cpp
        test/operatorstest.cpp
        src/slottedpages/spsegment.cpp
        src/slottedpages/slottedpages.cpp
        src/slottedpages/Record.cpp
        src/slottedpages/schemasegment.cpp
        )

# Executable
add_executable(DBLite_Esort ${ESORT_FILES})
add_executable(DBLite_Buffer ${BUFFER_FILES})
add_executable(DBLite_SPSegment ${SLOTTED_PAGE_FILES})
add_executable(DBLite_SCHEMA ${SCHEMA_FILES})
add_executable(DBLite_BTREE ${BTREE_FILES})
add_executable(DBLite_Operators ${OPERATORS_FILES})


target_include_directories(DBLite_Esort PUBLIC include/)
target_include_directories(DBLite_Buffer PUBLIC include/)
target_include_directories(DBLite_SPSegment PUBLIC include/)
target_include_directories(DBLite_SCHEMA PUBLIC include/)
target_include_directories(DBLite_BTREE PUBLIC include/)
target_include_directories(DBLite_Operators PUBLIC include/)


# Test
#add_custom_target(stest
#        ../test/test.sh
#        )

# doxygen build
set(DOXYGEN_TARGETS DBLite_Esort)
set(DOXYGEN_TARGETS DBLite_Buffer)
set(DOXYGEN_TARGETS DBLite_SPSegment)
set(DOXYGEN_TARGETS DBLite_SCHEMA)
set(DOXYGEN_TARGETS DBLite_BTREE)
set(DOXYGEN_TARGETS DBLite_Operators)


include(doc/docs.cmake)