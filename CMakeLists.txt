cmake_minimum_required(VERSION 3.5)
project(twes)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c99")

set(SOURCE_FILES twes.c server.c server.h utils.c utils.h http.c http.h)
add_executable(twes ${SOURCE_FILES})