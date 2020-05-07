cmake_minimum_required(VERSION 3.0)

project(i2c_tests)
enable_testing()

######### Compile tested modules as individual libraries #########

### adc_stack library ###
add_library(i2c_driver STATIC
    ../src/i2c.c
)
target_include_directories(i2c_driver PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Stub
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${AVR_INCLUDES}
)

target_compile_definitions(i2c_driver PRIVATE
    -DUNIT_TESTING
)

########## I2C driver tests ##########

add_executable(i2c_driver_tests
    i2c_driver_tests.cpp
)

target_compile_definitions(i2c_driver_tests PRIVATE
    -DUNIT_TESTING
)

target_include_directories(i2c_driver_tests PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Stub
    ${GTEST_INCLUDE_DIRS}
)
target_link_libraries(i2c_driver_tests i2c_driver ${GTEST_LIBRARIES} pthread)

set_target_properties(i2c_driver_tests
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/I2c
)
