cmake_minimum_required (VERSION 2.8)
project (Args)
 
# The version number.
set (Args_VERSION_MAJOR 0)
set (Args_VERSION_MINOR 1)

include(CheckCXXCompilerFlag)
enable_language(CXX)

install (FILES include/args.hpp DESTINATION include)

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -C ${CMAKE_CFG_INTDIR})

macro(add_test_executable TEST_NAME_)
    set(TEST_NAME "${TEST_NAME_}")
    add_executable (${TEST_NAME} EXCLUDE_FROM_ALL ${ARGN})
    if(WIN32)
        add_test(NAME ${TEST_NAME} WORKING_DIRECTORY ${LIBRARY_OUTPUT_PATH} COMMAND ${TEST_NAME}${CMAKE_EXECUTABLE_SUFFIX})
    else()
        add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
    endif()
    add_dependencies(check ${TEST_NAME})
    set_tests_properties(${TEST_NAME} PROPERTIES FAIL_REGULAR_EXPRESSION "FAILED")
endmacro(add_test_executable)

include(CTest)

include_directories(include)

file(GLOB TESTS test/*.cpp)
foreach(TEST_FILE ${TESTS})
    get_filename_component(BASE_NAME ${TEST_FILE} NAME_WE)
    add_test_executable(test_${BASE_NAME} ${TEST_FILE})
endforeach()

file(GLOB EXAMPLES examples/*.cpp)
foreach(EXAMPLE ${EXAMPLES})
    get_filename_component(BASE_NAME ${EXAMPLE} NAME_WE)
    message("BASE_NAME: ${BASE_NAME}")
    message("EXAMPLE: ${EXAMPLE}")
    add_executable(example_${BASE_NAME} EXCLUDE_FROM_ALL ${EXAMPLE})
endforeach()
