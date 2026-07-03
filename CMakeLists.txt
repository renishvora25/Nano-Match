cmake_minimum_required(VERSION 3.10)
project(NanoMatch VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Strict warnings and maximum optimization for the hot path
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -O3 -march=native")

include_directories(include)

add_executable(nano_match src/main.cpp)

target_link_libraries(nano_match pthread)