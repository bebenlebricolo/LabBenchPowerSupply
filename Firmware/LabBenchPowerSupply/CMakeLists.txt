cmake_minimum_required(VERSION 3.0)
project(LabBenchPowerSupply_Firmware)

find_package(GTest REQUIRED)
enable_testing()

# Set definitions
add_definitions(
    -D__AVR_ATmega328__
)

set (AVR_INCLUDES
    "/usr/avr/include"
    "/usr/lib/gcc/avr/9.3.0/include"
)

set (WARNING_FLAGS "-Wall -Wextra")

add_subdirectory(
    ${CMAKE_SOURCE_DIR}/Drivers
)
