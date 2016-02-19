cmake_minimum_required(VERSION 3.3)
project(nio)

include_directories("/usr/local/include/luajit-2.1")
link_directories("/usr/local/lib")
link_libraries("libluajit-5.1.so")

set(SOURCE_FILES nio.c)
add_library(nio SHARED ${SOURCE_FILES})
set_target_properties(nio PROPERTIES PREFIX "")
