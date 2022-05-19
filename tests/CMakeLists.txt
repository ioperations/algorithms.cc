enable_testing()
find_package(GTest REQUIRED)

set(MAIN_SRC ${CMAKE_CURRENT_SOURCE_DIR}/..)
include_directories(${MAIN_SRC}/inlcude)
include_directories(${MAIN_SRC}/include/graph)
link_libraries(GTest::GTest GTest::Main)

macro (do_add_test _name)
    add_executable(${_name} ${_name}.cpp)

    add_test(NAME ${_name} COMMAND ${_name} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    target_compile_options(${_name} PUBLIC "-g")
endmacro()

file(GLOB TEST_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*_test.cpp)
foreach(_path ${TEST_SOURCE_FILES})
    get_filename_component(_name ${_path} NAME)
    list(APPEND TEST_SOURCE_FILE_NAMES ${_name})
    get_filename_component(_we ${_path} NAME_WE)
    do_add_test(${_we})
endforeach()

string(JOIN ", " TEST_SOURCE_FILE_NAMES ${TEST_SOURCE_FILE_NAMES})
message(STATUS "Test files added: " ${TEST_SOURCE_FILE_NAMES})
