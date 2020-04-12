message("CMAKE_SYSTEM_LIBRARY_PATH = ${CMAKE_SYSTEM_LIBRARY_PATH}")
message("CMAKE_SYSTEM_INCLUDE_PATH = ${CMAKE_SYSTEM_INCLUDE_PATH}")

add_executable(firmware
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
)

target_include_directories(firmware PUBLIC
    ${CMAKE_SOURCE_DIR}/Drivers/Adc/inc
)

target_link_libraries(firmware
    adc_driver
)

add_extra_avr_targets(firmware)
