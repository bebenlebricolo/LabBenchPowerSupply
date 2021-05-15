cmake_minimum_required(VERSION 3.0)

project(timebase_module_tests)
enable_testing()

######### Compile tested modules as individual libraries #########


### timebase_module library ###
add_library(timebase_module STATIC
../src/timebase.c
)
target_include_directories(timebase_module PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../Drivers/Timers/Timer_generic/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../Drivers/Timers/Timer_8_bit/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../Drivers/Timers/Timer_8_bit_async/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../Drivers/Timers/Timer_16_bit/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/
)

########## Timebase module tests ##########

add_executable(timebase_module_tests
    timebase_tests.cpp
    Stubs/timer_8_bit_stub.c
    Stubs/timer_8_bit_async_stub.c
    Stubs/timer_16_bit_stub.c
)

target_include_directories(timebase_module_tests PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Stubs
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../Drivers/Timers/Timer_generic/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../Drivers/Timers/Timer_8_bit/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../Drivers/Timers/Timer_8_bit_async/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../Drivers/Timers/Timer_16_bit/inc
)

target_include_directories(timebase_module_tests SYSTEM PUBLIC
    ${GTEST_INCLUDE_DIRS}
)

target_link_libraries(timebase_module_tests
    timebase_module
    ${GTEST_LIBRARIES}

)

set_target_properties(timebase_module_tests
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/Modules/Timebase
)