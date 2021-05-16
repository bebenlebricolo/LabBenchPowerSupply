cmake_minimum_required(VERSION 3.0)

add_library(timebase_module STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/timebase.c
)

target_include_directories(timebase_module PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_SOURCE_DIR}/App/inc
    ${AVR_INCLUDES}
)

target_link_libraries(timebase_module
    timer_generic_driver
    timer_8_bit_driver
    timer_16_bit_driver
    timer_8_bit_async_driver
)