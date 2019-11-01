project(algorithms_in_cpp)
cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE DEBUG)

add_executable(stack stack.cpp)
link_libraries(print_utils)

add_subdirectory(tests)
