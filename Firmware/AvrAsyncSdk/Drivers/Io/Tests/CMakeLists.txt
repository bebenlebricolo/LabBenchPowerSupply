cmake_minimum_required(VERSION 3.0)

project(io_driver_tests)

######### Compile tested modules as individual libraries #########

### adc_stack library ###
add_library(io_driver STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/io.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Stub/config.c

)
target_include_directories(io_driver PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Stub
)

target_include_directories(io_driver SYSTEM PRIVATE
    ${AVR_INCLUDES}
)

target_compile_definitions(io_driver PRIVATE
    -DUNIT_TESTING
)

########## I2C driver tests ##########

add_executable(io_driver_tests
    io_tests.cpp
)

target_compile_definitions(io_driver_tests PRIVATE
    -DUNIT_TESTING
)

target_include_directories(io_driver_tests PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Stub
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_include_directories(io_driver_tests SYSTEM PUBLIC
    ${GTEST_INCLUDE_DIRS}
)

if(WIN32)
    target_link_libraries(io_driver_tests io_driver ${GTEST_LIBRARIES} )
else()
    target_link_libraries(io_driver_tests io_driver ${GTEST_LIBRARIES} pthread)
endif()

set_target_properties(io_driver_tests
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/Drivers/io
)
