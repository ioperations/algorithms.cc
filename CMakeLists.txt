
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

add_library(common_utils ./src/string_utils.cc ./src/tree_printer.cc ./src/text_block.cc ./src/array.cc ./src/rich_text.cc)
link_libraries(common_utils)

include_directories(PRIVATE ${CMAKE_SOURCE_DIR}/include)
include_directories(PRIVATE ${CMAKE_SOURCE_DIR}/third_party)

add_executable(stack ./src/stack.cc)
add_executable(quick_find ./src/quick_find.cc)
add_executable(sort ./src/sort.cc)
add_executable(complex_numbers ./src/complex_numbers.cc)
add_executable(hanoi ./src/hanoi.cc)
add_executable(other ./src/other.cc)
add_executable(radix ./src/radix.cc)
add_executable(ptree ./src/ptree.cc)

if(MSVC)
    # have to find and link additional modules for VC
    find_package(wxWidgets COMPONENTS core base adv xml html REQUIRED)
else()
    #find_package(wxWidgets COMPONENTS core base REQUIRED)
endif()

enable_testing()
add_subdirectory(tests)
