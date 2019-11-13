enable_testing()
find_package(GTest REQUIRED)

set(MAIN_SRC ${CMAKE_CURRENT_SOURCE_DIR}/..)
include_directories(${MAIN_SRC})
link_libraries(GTest::GTest GTest::Main)

macro (do_add_test _name)
    add_executable(${_name} ${_name}.cpp)
    add_test(NAME ${_name} COMMAND ./${_name})
endmacro()

do_add_test(tree_printer_test)
do_add_test(forward_list_test)
do_add_test(array_test)
do_add_test(stack_test)
do_add_test(fraction_test)

