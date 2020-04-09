cmake_minimum_required(VERSION 3.0)

project(Tests C CXX)

find_package(GTest REQUIRED)
enable_testing()

# Set definitions
add_definitions(
    -D__AVR_ATmega328__
)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_subdirectory(
    ${CMAKE_SOURCE_DIR}/../Firmware/Drivers/Adc/Tests 
        ${CMAKE_BINARY_DIR}/Tests
)