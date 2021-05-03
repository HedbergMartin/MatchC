cmake_minimum_required(VERSION 3.10.2)

set(target parsing)

add_library(${target} parser.c)

target_include_directories(${target} PUBLIC include)
target_link_libraries(${target} datatypes)