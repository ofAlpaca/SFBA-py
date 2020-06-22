cmake_minimum_required(VERSION 3.16)
project(SFBA)

set(CMAKE_CXX_STANDARD 14)

add_executable(SFBA main.cpp Topology.cpp Topology.h Tree.cpp Tree.h Node.cpp Node.h Slice.cpp Slice.h Config.hpp)
set_target_properties(SFBA PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}")