cmake_minimum_required(VERSION 3.0)

message("Configuring adc_driver static library : ")
message("  CMAKE_C_FLAGS : ${CMAKE_C_FLAGS}")
message("  CMAKE_CXX_FLAGS : ${CMAKE_CXX_FLAGS}")
message("  CMAKE_BUILD_TYPE : ${CMAKE_BUILD_TYPE}")
message("  AVR_MCU = : ${AVR_MCU}")

add_library(adc_driver STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/adc_stack.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/adc.c
)

target_include_directories(adc_driver PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/private_inc
    ${AVR_INCLUDES}
)