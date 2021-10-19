cmake_minimum_required(VERSION 3.12)

add_library(memutils STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/memutils.c
)

target_include_directories(memutils PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)