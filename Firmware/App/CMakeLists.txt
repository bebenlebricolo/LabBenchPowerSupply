set(FIRMWARE_NAME firmware)

message("CMAKE_SYSTEM_LIBRARY_PATH = ${CMAKE_SYSTEM_LIBRARY_PATH}")
message("CMAKE_SYSTEM_INCLUDE_PATH = ${CMAKE_SYSTEM_INCLUDE_PATH}")

add_executable(${FIRMWARE_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
)


target_include_directories(${FIRMWARE_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/Drivers/Adc/inc
    ${CMAKE_SOURCE_DIR}/Drivers/Adc/private_inc
    ${CMAKE_SOURCE_DIR}/Drivers/Peripheral/inc
)

target_link_directories( ${FIRMWARE_NAME} PUBLIC
    ${CMAKE_SYSTEM_LIBRARY_PATH}
    /usr/lib/gcc/avr/9.3.0/avr5
)

target_link_libraries(${FIRMWARE_NAME}
    adc_driver
    /usr/avr/lib/avr5/crtatmega328p.o
)

target_compile_options( ${FIRMWARE_NAME} PUBLIC
    -ffunction-sections
    -fdata-sections
    -fpack-struct
    -fshort-enums
    -funsigned-char
    -funsigned-bitfields
    -mmcu=${AVR_MCU}
    -DF_CPU=${AVR_MCU_SPEED}
)

add_extra_avr_targets(${FIRMWARE_NAME})
