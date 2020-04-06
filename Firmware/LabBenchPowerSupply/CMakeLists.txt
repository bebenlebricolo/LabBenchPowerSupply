cmake_minimum_required(VERSION 3.0)
project(LabBenchPowerSupply_tests)

find_package(GTest REQUIRED)
enable_testing()

# Set definitions
add_definitions(
    -D__AVR_ATmega328__
)


set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Og -g -fno-exceptions")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Og -g")
set (CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -flto")

set (AVR_INCLUDES
    "/usr/avr/include/avr"
    "/usr/lib/gcc/avr/9.3.0/include"
)

add_subdirectory(
    ${CMAKE_SOURCE_DIR}/Drivers/Adc/Tests
)
