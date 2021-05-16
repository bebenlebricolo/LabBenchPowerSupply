cmake_minimum_required(VERSION 3.20)
# Use AVR GCC toolchain
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/Toolchain/avr8-gcc-toolchain.cmake)

project(LabBenchPowerSupply_Firmware C)

set(AVR_MCU atmega328p)
set(AVR_MCU_SPEED 16000000)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

add_compile_definitions(
    F_CPU=${AVR_MCU_SPEED}
    __AVR_ATmega328P__
    I2C_IMPLEM_MASTER_FULL
)
add_compile_options(-mmcu=${AVR_MCU})

set(CMAKE_EXE_LINKER_FLAGS "-mmcu=${AVR_MCU}")

if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DDEBUG_WITH_SIMAVR)
endif()

# This variable is needed by I2c driver in order to locate the config.h file
# Provided by the application
set (CONFIG_FILE_DIR ${CMAKE_SOURCE_DIR}/App/inc)

add_subdirectory(${CMAKE_SOURCE_DIR}/Utils)
add_subdirectory(${CMAKE_SOURCE_DIR}/Drivers)
add_subdirectory(${CMAKE_SOURCE_DIR}/Modules)
add_subdirectory(${CMAKE_SOURCE_DIR}/App)
