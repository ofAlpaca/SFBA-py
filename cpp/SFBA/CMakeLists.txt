cmake_minimum_required(VERSION 3.16)
project(SFBA)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenMP)
if (OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif ()

add_library(Core Topology.cpp Topology.h Tree.cpp Tree.h Node.hpp Slice.cpp Slice.h Config.hpp Global.cpp Global.h SFBA.hpp Stellar.hpp)

add_executable(main test/newTest.cpp)
target_link_libraries(main Core)
set_target_properties(main PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}")