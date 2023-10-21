cmake_minimum_required(VERSION 3.26)
project(mirror_logging)

set(CMAKE_CXX_STANDARD 17)

add_library(mirror_logging logger.cpp)
target_link_libraries(mirror_logging zmq)
