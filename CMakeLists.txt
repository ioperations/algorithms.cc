project(algorithms_in_cpp)
cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE DEBUG)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wno-long-long -pedantic -O0")
endif()

add_executable(stack stack.cpp)
add_executable(quick_find quick_find.cpp tree_printer.cpp string_utils.cpp text_block.cpp)
add_executable(sort sort.cpp)

add_subdirectory(tests)
