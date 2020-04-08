cmake_minimum_required(VERSION 3.0)

project(adc_tests)
enable_testing()

########## Adc stack tests ##########
add_executable(adc_stack_tests
    adc_stack_tests.cpp
    ../src/adc_stack.c
)

target_compile_definitions(adc_stack_tests PRIVATE
    -DUNIT_TESTING
)

target_include_directories(adc_stack_tests PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../private_inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Peripheral/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Stub
    ${GTEST_INCLUDE_DIRS}
)
target_link_libraries(adc_stack_tests ${GTEST_LIBRARIES} pthread)

########## Adc driver tests ##########

add_executable(adc_driver_tests
    adc_driver_tests.cpp
    ../src/adc.c
    ../src/adc_stack.c
    Stub/adc_register_stub.c
)

target_compile_definitions(adc_driver_tests PRIVATE
    -DUNIT_TESTING
)

target_include_directories(adc_driver_tests PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../private_inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Stub
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Peripheral/inc
    ${GTEST_INCLUDE_DIRS}
)
target_link_libraries(adc_driver_tests ${GTEST_LIBRARIES} pthread)


