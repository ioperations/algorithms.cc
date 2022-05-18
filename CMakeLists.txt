
project(algorithms_in_cpp)
cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE DEBUG)


set(CXX_FLAGS     
    -g)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wno-long-long -pedantic -O0")
endif()

add_library(common_utils string_utils.cpp tree_printer.cpp text_block.cpp array.cpp rich_text.cpp)
link_libraries(common_utils)

include_directories(PRIVATE ${CMAKE_SOURCE_DIR})

add_executable(stack stack.cpp)
add_executable(quick_find quick_find.cpp)
add_executable(sort sort.cpp)
add_executable(complex_numbers complex_numbers.cpp)
add_executable(hanoi hanoi.cpp)
add_executable(other other.cpp)
add_executable(radix radix.cpp)
add_executable(ptree ptree.cc)

if(MSVC)
    # have to find and link additional modules for VC
    find_package(wxWidgets COMPONENTS core base adv xml html REQUIRED)
else()
    #find_package(wxWidgets COMPONENTS core base REQUIRED)
endif()

enable_testing()
add_subdirectory(tests)
