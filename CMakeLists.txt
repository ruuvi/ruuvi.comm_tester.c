cmake_minimum_required(VERSION 3.7)

project(ruuvi_comm_tester)
set(ProjectId ruuvi_comm_tester)

set(RUUVI_APP_SOURCES
        src/app/comm_tester.c
)

set(RUUVI_DRV_SOURCES
        src/drv/terminal.c
)

set(RUUVI_LIB_SOURCES
        src/lib/api.c
        src/lib/parser.c
        ../ruuvi.endpoints.c/src/ruuvi_endpoint_ca_uart.c
)

include_directories(
#        /usr/include
        src
        src/app
        src/lib
        src/drv
        ../ruuvi.endpoints.c/src
)


add_executable(${ProjectId}
        ${RUUVI_APP_SOURCES}
        ${RUUVI_DRV_SOURCES}
        ${RUUVI_LIB_SOURCES}
)

set_target_properties(${ProjectId} PROPERTIES
        C_STANDARD 11
        CXX_STANDARD 14
)

target_compile_definitions(${ProjectId} PUBLIC
        _DEFAULT_SOURCE
)

target_compile_options(${ProjectId} PUBLIC
        -c
        -Wall
        -pedantic
        -Wno-variadic-macros
        -Wno-long-long
        -Wno-shadow
)

target_link_libraries(${ProjectId}
        pthread
)
