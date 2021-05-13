cmake_minimum_required(VERSION 3.10.2)

add_subdirectory(datatypes)
add_subdirectory(matching)

add_executable(main matchc.c)

target_link_libraries(main datatypes matching)
