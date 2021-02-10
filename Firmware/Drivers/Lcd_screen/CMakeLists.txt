cmake_minimum_required(VERSION 3.0)

add_library(HD44780_lcd_driver STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HD44780_lcd.c
)

target_include_directories(HD44780_lcd_driver PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_SOURCE_DIR}/Drivers/I2c/inc
    ${CMAKE_SOURCE_DIR}/Modules/Timebase/inc
    ${AVR_INCLUDES}
)