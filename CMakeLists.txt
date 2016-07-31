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
    add_executable (${TEST_NAME} EXCLUDE_FROM_ALL test/${TEST_NAME}.cpp ${ARGN})
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

add_test_executable(args)
