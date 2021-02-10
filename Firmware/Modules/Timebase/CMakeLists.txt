cmake_minimum_required(VERSION 3.0)

add_library(timebase_module STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/timebase.c
)

target_include_directories(timebase_module PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_SOURCE_DIR}/App/inc
    ${CMAKE_SOURCE_DIR}/Drivers/Timers/Timer_generic/inc
    ${CMAKE_SOURCE_DIR}/Drivers/Timers/Timer_8_bit/inc
    ${CMAKE_SOURCE_DIR}/Drivers/Timers/Timer_16_bit/inc
    ${CMAKE_SOURCE_DIR}/Drivers/Timers/Timer_8_bit_async/inc
    ${AVR_INCLUDES}
)