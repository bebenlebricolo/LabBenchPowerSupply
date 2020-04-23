message("CMAKE_SYSTEM_LIBRARY_PATH = ${CMAKE_SYSTEM_LIBRARY_PATH}")
message("CMAKE_SYSTEM_INCLUDE_PATH = ${CMAKE_SYSTEM_INCLUDE_PATH}")

add_executable(firmware
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/driver_setup.c
)

target_include_directories(firmware PUBLIC
    ${CMAKE_SOURCE_DIR}/App/inc
    ${CMAKE_SOURCE_DIR}/Drivers/Adc/inc
    ${CMAKE_SOURCE_DIR}/Drivers/Timers/Timer_generic/inc
    ${CMAKE_SOURCE_DIR}/Drivers/Timers/Timer_8_bit/inc
    ${CMAKE_SOURCE_DIR}/Drivers/Timers/Timer_8_bit_async/inc
    ${CMAKE_SOURCE_DIR}/Drivers/Timers/Timer_16_bit/inc
    ${SIMAVR_INCLUDE_DIR}
)

target_link_libraries(firmware
    adc_driver
    timer_8_bit_driver
    timer_16_bit_driver
)

add_extra_avr_targets(firmware)
