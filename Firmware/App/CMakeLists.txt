add_executable(firmware
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/driver_setup.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/module_setup.c
)

target_include_directories(firmware PUBLIC
    ${CMAKE_SOURCE_DIR}/App/inc
    ${CMAKE_SOURCE_DIR}/Drivers/Adc/inc
    ${CMAKE_SOURCE_DIR}/Drivers/Timers/Timer_generic/inc
    ${CMAKE_SOURCE_DIR}/Drivers/Timers/Timer_8_bit/inc
    ${CMAKE_SOURCE_DIR}/Drivers/Timers/Timer_8_bit_async/inc
    ${CMAKE_SOURCE_DIR}/Drivers/Timers/Timer_16_bit/inc
    ${CMAKE_SOURCE_DIR}/Drivers/Lcd_screen/inc
    ${CMAKE_SOURCE_DIR}/Drivers/I2c/inc
    ${SIMAVR_INCLUDE_DIR}
)

target_link_libraries(firmware
    adc_driver
    timer_8_bit_driver
    timer_8_bit_async_driver
    timer_16_bit_driver
    i2c_driver
    timebase_module
    HD44780_lcd_driver
)

add_extra_avr_targets(firmware)
