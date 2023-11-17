cmake_minimum_required(VERSION 3.22)
project(mirror_logging VERSION 0.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

add_library(mirror_logging src/logger.cpp)
target_link_libraries(mirror_logging zmq)
target_include_directories(mirror_logging PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include)
