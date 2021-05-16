cmake_minimum_required(VERSION 3.0)

# Stop here as we need an extra config.h file in order to build this driver
# This file provides symbols for the driver to work properly
if(NOT DEFINED CONFIG_FILE_DIR)
    message(FATAL_ERROR " -- No config file directory was provided to I2C driver, please provide the \"CONFIG_FILE_DIR\" variable")
endif()

add_library(i2c_driver STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/i2c.c
)

target_include_directories(i2c_driver PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CONFIG_FILE_DIR}
    ${AVR_INCLUDES}
)