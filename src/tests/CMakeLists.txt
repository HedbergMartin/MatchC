cmake_minimum_required(VERSION 3.10)

add_executable(match_tests match_tests.c testing_utils.c)
target_link_libraries(match_tests datatypes matching ihct)