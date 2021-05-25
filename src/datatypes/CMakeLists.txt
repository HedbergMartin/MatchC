cmake_minimum_required(VERSION 3.10.2)

set(target datatypes)

add_library(${target} vector.c list.c queue.c hash_table_char.c subst_tree.c)

target_include_directories(${target} PUBLIC include)