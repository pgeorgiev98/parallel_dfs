cmake_minimum_required(VERSION 3.11)
project(dfs)
add_executable(dfs main.cpp arguments.cpp graph.cpp log.cpp)
set_property(TARGET dfs PROPERTY CXX_STANDARD 17)
