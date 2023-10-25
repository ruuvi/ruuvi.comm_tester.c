cmake_minimum_required(VERSION 3.7)

project(ruuvi_comm_tester-test-parser)
set(ProjectId ruuvi_comm_tester-test-parser)

add_executable(${ProjectId}
        test_parser.cpp
        ${SRC}/lib/parser.c
        ${SRC}/lib/parser.h
)

set_target_properties(${ProjectId} PROPERTIES
        C_STANDARD 11
        CXX_STANDARD 14
)

target_include_directories(${ProjectId} PUBLIC
        ${gtest_SOURCE_DIR}/include
        ${gtest_SOURCE_DIR}
        ${SRC}
        ${RUUVI_ENDPOINTS_SRC}
        include
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(${ProjectId} PUBLIC
        RUUVI_TESTS_PARSER=1
)

target_compile_options(${ProjectId} PUBLIC
        -g3
        -ggdb
        -fprofile-arcs
        -ftest-coverage
        --coverage
)

target_link_libraries(${ProjectId}
        gtest
        gtest_main
        gcov
        --coverage
)

