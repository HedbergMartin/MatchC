cmake_minimum_required(VERSION 3.10.2)

set(target matching)

add_library(${target} parserNew.c d_net.c flatterm.c subjectFlatterm.c s_vector.c match_entry.c)

target_include_directories(${target} PUBLIC include)
target_link_libraries(${target} datatypes)