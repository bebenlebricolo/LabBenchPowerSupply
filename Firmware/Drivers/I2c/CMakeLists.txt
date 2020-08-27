cmake_minimum_required(VERSION 3.0)

add_library(i2c_driver STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/i2c.c
)

target_include_directories(i2c_driver PUBLIC
    ${CMAKE_SOURCE_DIR}/App/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${AVR_INCLUDES}
)