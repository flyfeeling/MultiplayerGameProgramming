cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(Tests)

# Create executables
add_executable(main_01_client main_01_client.cpp)
target_link_libraries(main_01_client Multiplayer)

add_executable(main_01_server main_01_server.cpp)
target_link_libraries(main_01_server Multiplayer)

