cmake_minimum_required(VERSION 3.0)

find_program( AVR_CC avr-gcc )
find_program( AVR_CXX avr-g++ )
find_program( AVR_OBJCOPY avr-objcopy )
find_program( AVR_SIZE_TOOL avr-size )
find_program( AVR_OBJDUMP avr-objdump )

set( CMAKE_SYSTEM_NAME Generic )
set( CMAKE_SYSTEM_PROCESSOR avr )
set( CMAKE_C_COMPILER ${AVR_CC} )
set( CMAKE_CXX_COMPILER ${AVR_CXX} )

set(WARNING_FLAGS "-Wall -Wextra")
set(LINKER_OPTIONS_FLAGS "")

set (CMAKE_CXX_FLAGS_RELEASE "-O3 -fno-exceptions ${WARNING_FLAGS}")
set (CMAKE_C_FLAGS_RELEASE "-O3 ${WARNING_FLAGS}")
set (CMAKE_LINKER_FLAGS_RELEASE "${LINKER_OPTIONS_FLAGS}")

set (CMAKE_CXX_FLAGS_DEBUG "-Og  -g -fno-exceptions ${WARNING_FLAGS}")
set (CMAKE_C_FLAGS_DEBUG " -Og  -g ${WARNING_FLAGS} ${LINKER_OPTIONS_FLAGS} -Wl,--verbose")
set (CMAKE_LINKER_FLAGS_DEBUG "${LINKER_OPTIONS_FLAGS}")


##########################################################################
# check build types:
# - Debug
# - Release
# - MinSizeRel
# - RelWithDebInfo
#
# Release is chosen, because of some optimized functions in the
# AVR toolchain, e.g. _delay_ms().
##########################################################################
if( NOT ( (CMAKE_BUILD_TYPE MATCHES Release) OR
        (CMAKE_BUILD_TYPE MATCHES Debug)))
   set(
      CMAKE_BUILD_TYPE Release
      CACHE STRING "Choose cmake build type: Debug Release RelWithDebInfo MinSizeRel"
      FORCE
   )
endif( NOT ( (CMAKE_BUILD_TYPE MATCHES Release) OR
           (CMAKE_BUILD_TYPE MATCHES Debug)))



###########################################################################
# some cmake cross-compile necessities
##########################################################################
if( DEFINED ENV{AVR_FIND_ROOT_PATH} )
    set( CMAKE_FIND_ROOT_PATH $ENV{AVR_FIND_ROOT_PATH} )
else( DEFINED ENV{AVR_FIND_ROOT_PATH} )
    if( EXISTS "/opt/local/avr" )
      set( CMAKE_FIND_ROOT_PATH "/opt/local/avr" )
    elseif( EXISTS "/usr/local/avr" )
      set( CMAKE_FIND_ROOT_PATH "/usr/local/avr" )
    elseif( EXISTS "/usr/avr" )
      set( CMAKE_FIND_ROOT_PATH "/usr/avr" )
    elseif( EXISTS "/usr/local/avr" )
      set( CMAKE_FIND_ROOT_PATH "/usr/local/avr" )
    elseif( EXISTS "/usr/lib/avr" )
      set( CMAKE_FIND_ROOT_PATH "/usr/lib/avr" )
    else( EXISTS "/opt/local/avr" )
      message( FATAL_ERROR "Please set AVR_FIND_ROOT_PATH in your environment." )
    endif( EXISTS "/opt/local/avr" )
endif( DEFINED ENV{AVR_FIND_ROOT_PATH} )
set( CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER )
set( CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY )
set( CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY )
# not added automatically, since CMAKE_SYSTEM_NAME is "generic"
set( CMAKE_SYSTEM_INCLUDE_PATH "${CMAKE_FIND_ROOT_PATH}/include" )
set( CMAKE_SYSTEM_LIBRARY_PATH "${CMAKE_FIND_ROOT_PATH}/lib" )


##########################################################################
# status messages for generating
##########################################################################
message( STATUS "Set CMAKE_FIND_ROOT_PATH to ${CMAKE_FIND_ROOT_PATH}" )
message( STATUS "Set CMAKE_SYSTEM_INCLUDE_PATH to ${CMAKE_SYSTEM_INCLUDE_PATH}" )
message( STATUS "Set CMAKE_SYSTEM_LIBRARY_PATH to ${CMAKE_SYSTEM_LIBRARY_PATH}" )

### Function to add an avr executable
function ( add_avr_executable EXECUTABLE_NAME)

    add_executable( ${EXECUTABLE_NAME}.elf
        ${ARGN}
    )

endfunction( add_avr_executable)

### Function to add extra targets to executable
function ( add_extra_avr_targets EXECUTABLE_NAME )

    # Strip binary for upload
    add_custom_target(strip ALL avr-strip ${EXECUTABLE_NAME} DEPENDS ${EXECUTABLE_NAME})

    # Transform binary into hex file, we ignore the eeprom segments in the step
    add_custom_target(hex ALL avr-objcopy -R .eeprom -O ihex ${EXECUTABLE_NAME} ${EXECUTABLE_NAME}.hex DEPENDS strip)

    add_custom_target(size ALL avr-size ${EXECUTABLE_NAME} DEPENDS ${EXECUTABLE_NAME})

endfunction( add_extra_avr_targets )