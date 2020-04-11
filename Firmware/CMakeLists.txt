cmake_minimum_required(VERSION 3.0)
set (CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/Toolchain/avr-gcc-toolchain.cmake)

project(firmware C CXX ASM)

set(AVR_MCU "atmega328p")
set(AVR_MCU_SPEED 16000000)
add_definitions(-DF_CPU=${AVR_MCU_SPEED})


set(CMAKE_SYSTEM_LIBRARY_PATH /usr/avr/lib)
set(CMAKE_SYSTEM_INCLUDE_PATH /usr/avr/include)

# Set definitions
add_definitions(
    -D__AVR_ATmega328__
)

set (AVR_INCLUDES
    "/usr/avr/include"
    "/usr/lib/gcc/avr/9.3.0/include"
)

# Adding sources
add_subdirectory(${CMAKE_SOURCE_DIR}/Drivers)
add_subdirectory(${CMAKE_SOURCE_DIR}/App)
