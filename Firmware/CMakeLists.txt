cmake_minimum_required(VERSION 3.0)
# Use AVR GCC toolchain
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/Toolchain/avr-gcc-toolchain.cmake)

project(firmware C)

set(AVR_MCU atmega328p)
set(AVR_MCU_SPEED 16000000)

add_definitions(-DF_CPU=${AVR_MCU_SPEED})
add_compile_options(-mmcu=${AVR_MCU})

set(CMAKE_EXE_LINKER_FLAGS "-mmcu=${AVR_MCU}")

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -Wall -Wextra")
set(CMAKE_C_FLAGS_DEBUG "-O0 -g3 -Wall -Wextra")
#set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall -Wextra")
#set(CMAKE_C_FLAGS_RELEASE "-O3 -Wall -Wextra")

# Adding sources
add_subdirectory(${CMAKE_SOURCE_DIR}/Drivers)
add_subdirectory(${CMAKE_SOURCE_DIR}/Modules)
add_subdirectory(${CMAKE_SOURCE_DIR}/App)
