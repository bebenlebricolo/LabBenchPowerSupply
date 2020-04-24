cmake_minimum_required(VERSION 3.0)

add_library(timer_8_bit_driver STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/timer_8_bit.c
)

target_include_directories(timer_8_bit_driver PRIVATE
    ${CMAKE_SOURCE_DIR}/App/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../Timer_generic/inc
    ${AVR_INCLUDES}
)