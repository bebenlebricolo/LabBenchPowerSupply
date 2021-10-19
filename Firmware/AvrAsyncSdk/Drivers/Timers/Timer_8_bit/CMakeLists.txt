cmake_minimum_required(VERSION 3.0)

add_library(timer_8_bit_driver STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/timer_8_bit.c
)

if (NOT DEFINED CONFIG_FILE_DIR)
    message(FATAL_ERROR "No config file path was provided to timer_8_bit_async_driver, please provide the variable \"CONFIG_FILE_DIR\" to point to the config.h location")
endif()

target_include_directories(timer_8_bit_driver PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CONFIG_FILE_DIR}
    ${AVR_INCLUDES}
)

target_link_libraries(timer_8_bit_driver timer_generic_driver)