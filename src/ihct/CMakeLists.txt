cmake_minimum_required(VERSION 3.13)
project(ihct VERSION 0.11)

set(CMAKE_CC_FLAGS "-Wall -Wextra -pedantic -Wfatal-errors")
set(CMAKE_CC_STANDARD 99)
set(CMAKE_BUILD_TYPE Debug)

find_package(Threads REQUIRED)

add_library(ihct
    SHARED
    src/ihct.c
    src/vector.c
)
target_link_libraries(ihct PRIVATE Threads::Threads)
target_include_directories(ihct PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

set(inc_dest "include/")
set(lib_dest "lib/")
install(TARGETS ihct DESTINATION ${lib_dest})
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/ihct.h" DESTINATION ${inc_dest})
