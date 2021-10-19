cmake_minimum_required(VERSION 3.20)

project(LabBenchPowerSupply_FirmwareTests C CXX)

find_package(GTest REQUIRED)
enable_testing()

# Set definitions
add_definitions(
    -D__AVR_ATmega328__
    -DUNIT_TESTING
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
add_subdirectory( ${CMAKE_SOURCE_DIR}/../AvrAsyncCore/Tests
    ${CMAKE_BINARY_DIR}/Tests/AvrAsyncCore
)

# Thermistor driver
add_subdirectory( ${CMAKE_SOURCE_DIR}/../Sensors/Thermistor/Tests
    ${CMAKE_BINARY_DIR}/Tests/Sensors/Thermistor
)
