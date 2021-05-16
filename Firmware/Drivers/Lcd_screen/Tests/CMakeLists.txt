cmake_minimum_required(VERSION 3.0)

project(HD44780_lcd)
enable_testing()

######### Compile tested modules as individual libraries #########

### adc_stack library ###
add_library(HD44780_lcd_driver STATIC
    ../src/HD44780_lcd.c
)
target_include_directories(HD44780_lcd_driver PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Stub
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${AVR_INCLUDES}
)

target_compile_definitions(HD44780_lcd_driver PRIVATE
    -DUNIT_TESTING
)

########## I2C driver tests ##########

add_executable(HD44780_lcd_driver_tests
    HD44780_lcd_tests.cpp
    Stub/i2c_stub.c
    Stub/timebase_stub.c
)

target_compile_definitions(HD44780_lcd_driver_tests PRIVATE
    -DUNIT_TESTING
)

target_include_directories(HD44780_lcd_driver_tests PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Stub
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_include_directories(HD44780_lcd_driver_tests SYSTEM PUBLIC
    ${GTEST_INCLUDE_DIRS}
)

if(WIN32)
    target_link_libraries(HD44780_lcd_driver_tests HD44780_lcd_driver ${GTEST_LIBRARIES} )
else()
    target_link_libraries(HD44780_lcd_driver_tests HD44780_lcd_driver ${GTEST_LIBRARIES} pthread)
endif()

set_target_properties(HD44780_lcd_driver_tests
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/Drivers/Lcd_screen
)
