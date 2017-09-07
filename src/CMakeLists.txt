cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(Multiplayer)

# Compile sources into library
file(GLOB sources "*.cpp")

# Create library
add_library(${PROJECT_NAME} SHARED ${sources})

# Define headers for this library. PUBLIC headers are used for
# compiling the library, and will be added to consumers' build
# paths.
target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

add_subdirectory(tests)

