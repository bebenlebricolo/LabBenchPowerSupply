cmake_minimum_required(VERSION 3.0)

add_library(HD44780_lcd_driver STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HD44780_lcd.c
)

target_include_directories(HD44780_lcd_driver PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${AVR_INCLUDES}
)

target_link_libraries(HD44780_lcd_driver
    i2c_driver
    timebase_module
)