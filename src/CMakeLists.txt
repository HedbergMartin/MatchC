cmake_minimum_required(VERSION 3.10.2)

add_subdirectory(datatypes)
add_subdirectory(matching)
add_subdirectory(ihct)

add_executable(main matchc.c)
target_link_libraries(main datatypes matching)

add_executable(match_tests match_tests.c)
target_link_libraries(match_tests datatypes matching ihct)
