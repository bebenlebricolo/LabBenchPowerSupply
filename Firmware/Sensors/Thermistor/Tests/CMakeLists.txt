cmake_minimum_required(VERSION 3.0)

project(thermistor_test)

######### Compile tested modules as individual libraries #########

### thermistor_driver library ###
add_library(thermistor_driver STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/thermistor.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/thermistor_private.c
)
target_include_directories(thermistor_driver PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../private_inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../Drivers/Adc/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Stubs
)

target_compile_definitions(thermistor_driver PRIVATE
    -DUNIT_TESTING
)


########## Thermistor driver tests ##########

add_executable(thermistor_driver_test
    ${CMAKE_CURRENT_SOURCE_DIR}/thermistor_driver_tests.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Stubs/config.c
)

target_compile_definitions(thermistor_driver_test PRIVATE
    -DUNIT_TESTING
)

target_include_directories(thermistor_driver_test PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Stubs
)

target_include_directories(thermistor_driver_test SYSTEM PUBLIC
    ${GTEST_INCLUDE_DIRS}
)

if(WIN32)
    target_link_libraries(thermistor_driver_test thermistor_driver ${GTEST_LIBRARIES})
else()
    target_link_libraries(thermistor_driver_test thermistor_driver ${GTEST_LIBRARIES} pthread)
endif()

set_target_properties(thermistor_driver_test
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/Thermistor
)
