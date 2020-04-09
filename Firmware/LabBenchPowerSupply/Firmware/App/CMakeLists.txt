add_executable(firmware
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
)

target_include_directories(firmware PUBLIC
    ${CMAKE_SOURCE_DIR}/Drivers/Adc/inc
    ${CMAKE_SOURCE_DIR}/Drivers/Peripheral/inc
)

target_link_libraries(firmware 
    adc_driver
)