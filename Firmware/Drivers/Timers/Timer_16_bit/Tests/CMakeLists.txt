cmake_minimum_required(VERSION 3.0)

project(timer_16_bit_driver_test)
enable_testing()

######### Compile tested modules as individual libraries #########

### timer_16_bit_driver library ###
add_library(timer_16_bit_driver STATIC
    ../src/timer_16_bit.c
)
target_include_directories(timer_16_bit_driver PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Timer_generic/inc
    ${CMAKE_CURRENT_SOURCE_DIR}
)

########## Timer 16 bit driver tests ##########

add_executable(timer_16_bit_driver_tests
    timer_16_bit_tests.cpp
    Stub/timer_16_bit_registers_stub.c
)

target_include_directories(timer_16_bit_driver_tests PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Stub
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Timer_generic/inc
    ${GTEST_INCLUDE_DIRS}
)
target_link_libraries(timer_16_bit_driver_tests timer_16_bit_driver timer_generic_driver ${GTEST_LIBRARIES} pthread)

set_target_properties(timer_16_bit_driver_tests
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/Drivers/Timers/
)