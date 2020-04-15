cmake_minimum_required(VERSION 3.0)

project(timer_drivers_test)
enable_testing()

######### Compile tested modules as individual libraries #########

### timer_8_bit_driver library ###
add_library(timer_8_bit_driver STATIC
    ../Timer_8_bit/src/timer_8_bit.c
)
target_include_directories(timer_8_bit_driver PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../Timer_8_bit/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../Timer_generic/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/
)

########## Timer 8 bit driver tests ##########

add_executable(timer_8_bit_driver_tests
    timer_8_bit_tests.cpp
    Stub/timer_8_bit_registers_stub.c
)

target_include_directories(timer_8_bit_driver_tests PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Stub
    ${CMAKE_CURRENT_SOURCE_DIR}/../Timer_8_bit/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../Timer_generic/inc
    ${GTEST_INCLUDE_DIRS}
)
target_link_libraries(timer_8_bit_driver_tests timer_8_bit_driver ${GTEST_LIBRARIES} pthread)

set_target_properties(timer_8_bit_driver_tests
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/Timers/
)
