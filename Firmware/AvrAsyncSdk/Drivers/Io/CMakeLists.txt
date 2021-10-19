cmake_minimum_required(VERSION 3.0)

add_library(io_driver STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/io.c
)

target_include_directories(io_driver PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CONFIG_FILE_DIR}
    ${AVR_INCLUDES}
)