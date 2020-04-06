cmake_minimum_required(VERSION 3.0)

project(adc_tests)


add_executable(adc_tests
    adc_tests.cpp
    ../src/adc_stack.c
)

target_include_directories(adc_tests PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../private_inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Peripheral/inc
    ${GTEST_INCLUDE_DIRS}
)
target_link_libraries(adc_tests ${GTEST_LIBRARIES} pthread)



