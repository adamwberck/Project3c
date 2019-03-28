cmake_minimum_required(VERSION 3.12)
project(Project3c C)

set(CMAKE_C_STANDARD 99)

add_executable(Project3c my_server.c helper.c helper.h my_server.h)