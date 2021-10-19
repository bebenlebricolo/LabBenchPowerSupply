cmake_minimum_required(VERSION 3.20)

project(LabBenchPowerSupply_FirmwareTests C CXX)

find_package(GTest REQUIRED)
enable_testing()

# Set definitions
add_definitions(
    -D__AVR_ATmega328__
    -DUNIT_TESTING
    -DI2C_IMPLEM_FULL_DRIVER
)

if(UNIX)
    set(AVR_INCLUDES /usr/avr/include)
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -Wall -Wextra --coverage")
    set(CMAKE_C_FLAGS_DEBUG "-O0 -g3 -Wall -Wextra --coverage")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall -Wextra --coverage")
    set(CMAKE_C_FLAGS_RELEASE "-O3 -Wall -Wextra --coverage")
else()
    if (NOT DEFINED AVR_INCLUDES)
        message(FATAL_ERROR "AVR_INCLUDES variable not set, please provide the path to avr includes")
    endif()
    set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} /Od /W4")
    set(CMAKE_C_FLAGS_DEBUG     "${CMAKE_C_FLAGS_DEBUG} /Od /W4")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /W4 ")
    set(CMAKE_C_FLAGS_RELEASE   "${CMAKE_C_FLAGS_RELEASE} /O2 /W4")
endif()



set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_subdirectory( ${CMAKE_SOURCE_DIR}/../Drivers/Adc/Tests
    ${CMAKE_BINARY_DIR}/Tests/Adc
)

# Memory utils library
add_subdirectory( ${CMAKE_SOURCE_DIR}/../Utils
    ${CMAKE_BINARY_DIR}/Tests/Utils
)

# Timer drivers
add_subdirectory( ${CMAKE_SOURCE_DIR}/../Drivers/Timers/Timer_generic/Tests
    ${CMAKE_BINARY_DIR}/Tests/Drivers/Timers/Timer_generic
)
add_subdirectory( ${CMAKE_SOURCE_DIR}/../Drivers/Timers/Timer_8_bit/Tests
    ${CMAKE_BINARY_DIR}/Tests/Drivers/Timers/Timer_8_bit
)
add_subdirectory( ${CMAKE_SOURCE_DIR}/../Drivers/Timers/Timer_8_bit_async/Tests
    ${CMAKE_BINARY_DIR}/Tests/Drivers/Timers/Timer_8_bit_async
)
add_subdirectory( ${CMAKE_SOURCE_DIR}/../Drivers/Timers/Timer_16_bit/Tests
    ${CMAKE_BINARY_DIR}/Tests/Drivers/Timers/Timer_16_bit
)

# I2C driver
add_subdirectory( ${CMAKE_SOURCE_DIR}/../Drivers/I2c/Tests
    ${CMAKE_BINARY_DIR}/Tests/Drivers/I2c
)

# LCD Screen HD44780 driver
add_subdirectory( ${CMAKE_SOURCE_DIR}/../Drivers/Lcd_screen/Tests
    ${CMAKE_BINARY_DIR}/Tests/Drivers/Lcd_screen
)

# Io driver
add_subdirectory( ${CMAKE_SOURCE_DIR}/../Drivers/Io/Tests
    ${CMAKE_BINARY_DIR}/Tests/Drivers/Io
)

# Thermistor driver
add_subdirectory( ${CMAKE_SOURCE_DIR}/../Sensors/Thermistor/Tests
    ${CMAKE_BINARY_DIR}/Tests/Sensors/Thermistor
)

# Modules
add_subdirectory( ${CMAKE_SOURCE_DIR}/../Modules/Timebase/Tests
    ${CMAKE_BINARY_DIR}/Tests/Modules/Timebase
)