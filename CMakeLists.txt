project(algorithms_in_cpp)
cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE DEBUG)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wno-long-long -pedantic -O0")
endif()

add_library(common_utils string_utils.cpp tree_printer.cpp text_block.cpp array.cpp)
link_libraries(common_utils)

add_executable(stack stack.cpp)
add_executable(quick_find quick_find.cpp)
add_executable(sort sort.cpp)
add_executable(complex complex.cpp)
add_executable(hanoi hanoi.cpp)

add_subdirectory(tests)
