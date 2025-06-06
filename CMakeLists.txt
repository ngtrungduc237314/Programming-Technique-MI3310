cmake_minimum_required(VERSION 3.10)
project(Program C)

include_directories(include)

file(GLOB SOURCES "src/*.c")

add_executable(program ${SOURCES})