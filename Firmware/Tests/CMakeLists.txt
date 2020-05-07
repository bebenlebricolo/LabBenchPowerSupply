cmake_minimum_required(VERSION 3.0)

project(Tests C CXX)

find_package(GTest REQUIRED)
enable_testing()

# Set definitions
add_definitions(
    -D__AVR_ATmega328__
    -DUNIT_TESTING
)

set(AVR_INCLUDES /usr/avr/include)

set(CMAKE_CXX_FLAGS_DEBUG "-Og -g -Wall -Wextra")
set(CMAKE_C_FLAGS_DEBUG "-Og -g -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall -Wextra")
set(CMAKE_C_FLAGS_RELEASE "-O3 -Wall -Wextra")


set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_subdirectory( ${CMAKE_SOURCE_DIR}/../Drivers/Adc/Tests
    ${CMAKE_BINARY_DIR}/Tests/Adc
)

add_subdirectory( ${CMAKE_SOURCE_DIR}/../Drivers/Timers/Timer_8_bit/Tests
    ${CMAKE_BINARY_DIR}/Tests/Timers/Timer_8_bit
)
add_subdirectory( ${CMAKE_SOURCE_DIR}/../Drivers/Timers/Timer_8_bit_async/Tests
    ${CMAKE_BINARY_DIR}/Tests/Timers/Timer_8_bit_async
)
add_subdirectory( ${CMAKE_SOURCE_DIR}/../Drivers/Timers/Timer_16_bit/Tests
    ${CMAKE_BINARY_DIR}/Tests/Timers/Timer_16_bit
)

add_subdirectory( ${CMAKE_SOURCE_DIR}/../Drivers/I2c/Tests
    ${CMAKE_BINARY_DIR}/Tests/I2c
)
