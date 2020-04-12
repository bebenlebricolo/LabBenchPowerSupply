set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/Toolchain/avr-gcc-toolchain.cmake)

cmake_minimum_required(VERSION 3.0)
# Use AVR GCC toolchain
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_CXX_COMPILER avr-g++)
set(CMAKE_C_COMPILER avr-gcc)

project(firmware C)
set(AVR_MCU atmega328p)
set(AVR_MCU_SPEED 16000000)

add_definitions(-DF_CPU=${AVR_MCU_SPEED})
add_compile_options(
    -mmcu=${AVR_MCU} # MCU
)

set(CMAKE_SYSTEM_LIBRARY_PATH /usr/avr/lib)
set(CMAKE_SYSTEM_INCLUDE_PATH /usr/avr/include)
set(CMAKE_EXE_LINKER_FLAGS "-mmcu=${AVR_MCU}")

# Adding sources
add_subdirectory(${CMAKE_SOURCE_DIR}/Drivers)
add_subdirectory(${CMAKE_SOURCE_DIR}/App)
