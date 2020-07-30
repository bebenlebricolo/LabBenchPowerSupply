cmake_minimum_required(VERSION 3.0)

project(timer_generic_tests)
enable_testing()

######### Compile tested modules as individual libraries #########

### timer_generic_driver library ###
add_library(timer_generic_driver STATIC
    ../src/timer_generic.c
)
target_include_directories(timer_generic_driver PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc
    ${CMAKE_CURRENT_SOURCE_DIR}/
)

########## Timer 8 bit driver tests ##########

add_executable(timer_generic_driver_tests
    timer_generic_tests.cpp
)

target_include_directories(timer_generic_driver_tests PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Timer_8_bit/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Timer_8_bit_async/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Timer_16_bit/inc
    ${GTEST_INCLUDE_DIRS}
)
target_link_libraries(timer_generic_driver_tests timer_generic_driver ${GTEST_LIBRARIES} pthread)

set_target_properties(timer_generic_driver_tests
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/Timers/
)