cmake_minimum_required(VERSION 3.16)
project(SFBA)

set(CMAKE_CXX_STANDARD 14)

add_executable(SFBA_Performance test/Performance.cpp Topology.cpp Topology.h Tree.cpp Tree.h Node.cpp Node.h Slice.cpp Slice.h Config.hpp Global.cpp Global.h)
set_target_properties(SFBA_Performance PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}")

add_executable(SFBA_Liveness test/Liveness.cpp Topology.cpp Topology.h Tree.cpp Tree.h Node.cpp Node.h Slice.cpp Slice.h Config.hpp Global.cpp Global.h)
set_target_properties(SFBA_Liveness PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}")