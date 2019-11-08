enable_testing()
find_package(GTest REQUIRED)

set(MAIN_SRC ${CMAKE_CURRENT_SOURCE_DIR}/..)
include_directories(${MAIN_SRC})
link_libraries(GTest::GTest GTest::Main)

add_executable(tree_printer_test tree_printer_test.cpp ${MAIN_SRC}/tree_printer.cpp ${MAIN_SRC}/string_utils.cpp)
add_executable(forward_list_test forward_list_test.cpp)
add_executable(array_test array_test.cpp ${MAIN_SRC}/array.cpp)
