cmake_minimum_required(VERSION 3.10.2)

set(target d_net)

add_library(${target} d_net.c)

target_include_directories(${target} PUBLIC include)
target_link_libraries(${target} datatypes)