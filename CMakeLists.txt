project(algorithms_in_cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE DEBUG)

add_executable(stack stack.cpp)
add_executable(tree tree.cpp)
link_libraries(print_utils)
