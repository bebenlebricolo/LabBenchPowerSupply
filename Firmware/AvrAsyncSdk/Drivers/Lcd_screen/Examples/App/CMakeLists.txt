
add_executable(lcd_screen_example
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/driver_setup.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/module_setup.c
)

# Produce a Map file
set (Map_file lcd_screen_example.map)

# Disable full debug optimizations for now
string(REPLACE "-Og" "-O0" CMAKE_C_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG})

set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wl,-Map=${Map_file}")
set (CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wl,-Map=${Map_file}")
set (CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL} -Wl,-Map=${Map_file}")
set (CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -Wl,-Map=${Map_file}")

target_include_directories(lcd_screen_example PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/App/inc
)

target_link_libraries(lcd_screen_example
    timer_8_bit_async_driver
    i2c_driver
    timebase_module
    HD44780_lcd_driver
)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT lcd_screen_example)

# Only add those targets if this is used on a Linux environment, as Atmel Studio already handles them itself

if (CMAKE_HOST_UNIX)
    message("-- Dealing with extra targets on Linux")
    add_extra_avr_targets(lcd_screen_example)
endif()
