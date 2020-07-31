cmake_minimum_required(VERSION 3.0)

add_library(timer_16_bit_driver STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/timer_16_bit.c
)

target_include_directories(timer_16_bit_driver PRIVATE
    ${CMAKE_SOURCE_DIR}/App/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../Timer_generic/inc
    ${AVR_INCLUDES}
)

target_link_libraries(timer_16_bit_driver timer_generic_driver)