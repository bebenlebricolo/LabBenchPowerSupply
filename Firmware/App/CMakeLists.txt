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
)

target_link_libraries(${FIRMWARE_NAME}
    adc_driver
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

## Rename the output to .elf as we will create multiple files
#set_target_properties(${FIRMWARE_NAME} PROPERTIES CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
#set_target_properties(${FIRMWARE_NAME} PROPERTIES EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
#set_target_properties(${FIRMWARE_NAME} PROPERTIES OUTPUT_NAME ${FIRMWARE_NAME}.elf )
#
## Strip binary for upload
#add_custom_target(strip ALL avr-strip ${FIRMWARE_NAME}.elf DEPENDS ${FIRMWARE_NAME})
#
## Transform binary into hex file, we ignore the eeprom segments in the step
#add_custom_target(hex ALL avr-objcopy -R .eeprom -O ihex ${FIRMWARE_NAME}.elf ${FIRMWARE_NAME}.hex DEPENDS strip)
#
#add_custom_target(size ALL avr-size ${FIRMWARE_NAME}.elf DEPENDS ${FIRMWARE_NAME})