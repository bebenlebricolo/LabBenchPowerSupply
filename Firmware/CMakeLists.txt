cmake_minimum_required(VERSION 3.0)

find_program( AVR_CC avr-gcc )
find_program( AVR_CXX avr-g++ )
find_program( AVR_OBJCOPY avr-objcopy )
find_program( AVR_SIZE_TOOL avr-size )
find_program( AVR_OBJDUMP avr-objdump )

set( CMAKE_SYSTEM_NAME Generic )
set( CMAKE_SYSTEM_PROCESSOR avr )
set( CMAKE_C_COMPILER ${AVR_CC} )
set( CMAKE_CXX_COMPILER ${AVR_CXX} )

project(firmware C CXX)
set(PRODUCT_NAME LabBenchPowerSupply)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -fno-exceptions ${WARNING_FLAGS}")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 ${WARNING_FLAGS}")
set (CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -flto")

set(AVR_MCU "atmega328p")
set(AVR_MCU_SPEED 16000000)
add_definitions(-DF_CPU=${AVR_MCU_SPEED})


# Set definitions
add_definitions(
    -D__AVR_ATmega328__
)

set (AVR_INCLUDES
    "/usr/avr/include"
    "/usr/lib/gcc/avr/9.3.0/include"
)

set (WARNING_FLAGS "-Wall -Wextra")

# Adding sources
add_subdirectory(${CMAKE_SOURCE_DIR}/Drivers)
add_subdirectory(${CMAKE_SOURCE_DIR}/App)
