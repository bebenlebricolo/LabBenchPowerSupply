cmake_minimum_required(VERSION 3.0)
project(adc_driver)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -fno-exceptions ${WARNING_FLAGS}")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 ${WARNING_FLAGS}")
set (CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -flto")

add_library(adc_driver STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/adc_stack.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/adc.c
)

target_include_directories(adc_driver PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/private_inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../Peripheral/inc
    ${AVR_INCLUDES}
)

##### registering tests #####
add_subdirectory(
    ${CMAKE_CURRENT_SOURCE_DIR}/Tests
)