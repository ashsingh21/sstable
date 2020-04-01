project(sstable)
cmake_minimum_required(VERSION 3.2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -g")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Build
add_executable(sstable
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)