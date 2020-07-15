cmake_minimum_required(VERSION 3.16)
project(SFBA)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenMP)
if (OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif ()
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_library(Core Topology.cpp Topology.h Tree.cpp Tree.h Node.hpp Slice.cpp Slice.h Config.hpp Global.cpp Global.h SFBA.hpp Stellar.hpp Dij.hpp)

add_executable(main test/newTest.cpp)
target_link_libraries(main Core)
set_target_properties(main PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}")

add_executable(main3 test/newTest2.cpp)
target_link_libraries(main3 Core)
set_target_properties(main3 PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}")


add_executable(main2 test/newTestStellar.cpp)
target_link_libraries(main2 Core)
set_target_properties(main2 PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}")


add_executable(caf test/cascadingFailure.cpp)
target_link_libraries(caf Core)
set_target_properties(caf PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}")

add_executable(10witness test/10witness.cpp)
target_link_libraries(10witness Core)
set_target_properties(10witness PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}")

add_executable(25witness test/25witness.cpp)
target_link_libraries(25witness Core)
set_target_properties(25witness PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}")

add_executable(50witness test/50witness.cpp)
target_link_libraries(50witness Core)
set_target_properties(50witness PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}")

add_executable(100witness test/100witness.cpp)
target_link_libraries(100witness Core)
set_target_properties(100witness PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}")