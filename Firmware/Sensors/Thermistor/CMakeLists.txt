cmake_minimum_required(VERSION 3.0)

add_library(thermistor_driver STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/thermistor.c
)

target_include_directories(thermistor_driver PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CONFIG_FILE_DIR}
    ${AVR_INCLUDES}
)

target_include_directories(thermistor_driver PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/private_inc
    ${CONFIG_FILE_DIR}
    ${AVR_INCLUDES}
)