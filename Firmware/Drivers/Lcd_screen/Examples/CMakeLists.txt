cmake_minimum_required( VERSION 3.18)
# Use AVR GCC toolchain
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/../../../Toolchain/avr8-gcc-toolchain.cmake)
project(lcd_screen_example C)

# Microcontroller variables
set(AVR_MCU atmega328p)
set(AVR_MCU_SPEED 16000000)
set(CMAKE_EXE_LINKER_FLAGS "-mmcu=${AVR_MCU}")

# Language standards
set(CMAKE_C_STANDARD 11)

# Required by avr-gcc toolchains
add_compile_definitions(
    F_CPU=${AVR_MCU_SPEED}
    __AVR_ATmega328P__
)
add_compile_options(-mmcu=${AVR_MCU})


# Target definitions
set( CONFIG_FILE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/App/inc )

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../  ${CMAKE_BINARY_DIR}/lcd_screen_example/Drivers/Lcd_screen)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../I2c  ${CMAKE_BINARY_DIR}/lcd_screen_example/Drivers/I2c)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../Timers  ${CMAKE_BINARY_DIR}/lcd_screen_example/Drivers/Timers)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../../Modules/Timebase  ${CMAKE_BINARY_DIR}/lcd_screen_example/Modules/Timebase)
add_subdirectory(App ${CMAKE_BINARY_DIR}/lcd_screen_example/bin )
