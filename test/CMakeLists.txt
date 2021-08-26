# (C) 2021 The University of Chicago
# See COPYRIGHT in top-level directory.
cmake_minimum_required (VERSION 3.13)
project (pycolza-test-pipeline CXX)

set (CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

find_package (colza REQUIRED)
find_package (spdlog REQUIRED)

add_library (pycolza-test-pipeline SHARED TestPipeline.cpp)
target_link_libraries (pycolza-test-pipeline colza-server spdlog::spdlog)
