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
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall  -g -O1 -pthread ")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -march=native -std=c++11 -g -O3 -pthread")
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
        include/dblite.h
        include/operations.hpp
        src/buffer/buffermanager.cpp
        include/buffer/buffermanager.h
        src/buffer/bufferframe.cpp
        include/buffer/bufferframe.h src/slottedpages/spsegment.cpp include/slottedpages/spsegment.h src/slottedpages/slottedpages.cpp include/slottedpages/slottedpages.h include/slottedpages/schema/Record.h src/slottedpages/Record.cpp include/slottedpages/tid.h include/slottedpages/schemasegment.h include/slottedpages/segment.h src/slottedpages/schemasegment.cpp src/slottedpages/segment.cpp)

# Source files
set(SLOTTED_PAGE_FILES
        test/slottedTest.cpp
        src/operations.cpp
        ${version_file}
        include/dblite.h
        include/operations.hpp
        src/buffer/buffermanager.cpp
        include/buffer/buffermanager.h
        src/buffer/bufferframe.cpp
        include/buffer/bufferframe.h src/slottedpages/spsegment.cpp include/slottedpages/spsegment.h src/slottedpages/slottedpages.cpp include/slottedpages/slottedpages.h include/slottedpages/schema/Record.h src/slottedpages/Record.cpp include/slottedpages/tid.h include/slottedpages/segment.h include/slottedpages/schemasegment.h src/slottedpages/segment.cpp src/slottedpages/schemasegment.cpp)

set(SCHEMA_FILES
        test/schematest.cpp
        src/operations.cpp
        ${version_file}
        include/dblite.h
        include/operations.hpp
        src/buffer/buffermanager.cpp
        include/buffer/buffermanager.h
        src/buffer/bufferframe.cpp
        include/buffer/bufferframe.h src/slottedpages/spsegment.cpp include/slottedpages/spsegment.h src/slottedpages/slottedpages.cpp include/slottedpages/slottedpages.h include/slottedpages/schema/Record.h src/slottedpages/Record.cpp include/slottedpages/tid.h include/slottedpages/segment.h include/slottedpages/schemasegment.h src/slottedpages/segment.cpp src/slottedpages/schemasegment.cpp include/slottedpages/schema/Parser.cpp include/slottedpages/schema/Parser.hpp include/slottedpages/schema/Schema.hpp include/slottedpages/schema/Schema.cpp)


# Executable
add_executable(DBLite_Esort ${ESORT_FILES})

add_executable(DBLite_Buffer ${BUFFER_FILES})

add_executable(DBLite_SPSegment ${SLOTTED_PAGE_FILES})

add_executable(DBLite_SCHEMA ${SCHEMA_FILES})




target_include_directories(DBLite_Esort PUBLIC include/)
target_include_directories(DBLite_Buffer PUBLIC include/)
target_include_directories(DBLite_SPSegment PUBLIC include/)
target_include_directories(DBLite_SCHEMA PUBLIC include/)



# Test
add_custom_target(stest
        ../test/test.sh
        )

# doxygen build
set(DOXYGEN_TARGETS DBLite_Esort)
set(DOXYGEN_TARGETS DBLite_Buffer)
set(DOXYGEN_TARGETS DBLite_SPSegment)
set(DOXYGEN_TARGETS DBLite_SCHEMA)


include(doc/docs.cmake)