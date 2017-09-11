cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(MultiplayerGameProgramming)


########################################################################
# System detection
########################################################################
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#include(CMake/MovingVisSystem.cmake)



########################################################################
# Dependencies management
########################################################################
#include(CMake/MovingVisDependencies.cmake)

#add_definitions(${MV_DEFINITIONS})



########################################################################
# Main targets
########################################################################
add_subdirectory(src)

