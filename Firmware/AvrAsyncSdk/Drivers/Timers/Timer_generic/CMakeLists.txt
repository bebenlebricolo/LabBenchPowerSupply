cmake_minimum_required(VERSION 3.0)

add_library(timer_generic_driver STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/timer_generic.c
)

target_include_directories(timer_generic_driver PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${AVR_INCLUDES}
)