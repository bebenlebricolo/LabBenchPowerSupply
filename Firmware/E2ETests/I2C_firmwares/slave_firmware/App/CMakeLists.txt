add_executable(slave_firmware
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/i2c_slave_handler.c
)

target_include_directories(slave_firmware PUBLIC
)

target_include_directories(slave_firmware PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_SOURCE_DIR}/../../../Drivers/I2c/inc
)

target_link_libraries(slave_firmware
    i2c_driver
)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT slave_firmware)

# Only add those targets if this is used on a Linux environment, as Atmel Studio already handles them itself

if (CMAKE_HOST_UNIX)
    message("-- Dealing with extra targets on Linux")
    add_extra_avr_targets(slave_firmware)
endif()
