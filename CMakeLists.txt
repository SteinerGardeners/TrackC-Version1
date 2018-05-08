cmake_minimum_required (VERSION 2.6)
project (Gardeners)

SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=gnu++11" )
add_library(GraphCommon edge.cpp graph.cpp tree.cpp listgraph.cpp)
add_library(TrackC init.cpp opt.cpp)
add_executable(submission main.cpp)
target_link_libraries (submission GraphCommon TrackC)
