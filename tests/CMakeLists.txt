cmake_minimum_required(VERSION 3.5)

project(ruuvi_comm_tester-tests)

add_definitions(
        -DRUUVI_TESTS=1
        -D_GNU_SOURCE
)

SET(SRC ${CMAKE_CURRENT_SOURCE_DIR}/../src)
SET(RUUVI_ENDPOINTS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/../components/ruuvi.endpoints.c/src)


include_directories(
	${SRC}/lib
)

enable_testing()
add_subdirectory(googletest)

add_subdirectory(test_parser)

add_test(NAME test_parser
        COMMAND ruuvi_comm_tester-test-parser
            --gtest_output=xml:$<TARGET_FILE_DIR:ruuvi_comm_tester-test-parser>/gtestresults.xml
)

