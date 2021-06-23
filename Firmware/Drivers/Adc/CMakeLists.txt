cmake_minimum_required(VERSION 3.0)

add_library(adc_driver STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/adc_stack.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/adc.c
)

target_include_directories(adc_driver PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/private_inc
    ${CONFIG_FILE_DIR}
    ${AVR_INCLUDES}
)