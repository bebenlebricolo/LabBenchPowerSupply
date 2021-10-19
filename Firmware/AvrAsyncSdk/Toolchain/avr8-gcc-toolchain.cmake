cmake_minimum_required(VERSION 3.3)

# see CMAKE_SYSTEM_NAME for cross compiling and Cmake system version
if(WIN32 AND NOT CMAKE_GENERATOR STREQUAL "Unix Makefiles")
  set( CMAKE_SYSTEM_NAME "BareMetal" )
  set( CMAKE_SYSTEM_VERSION "Generic" )
  set( CMAKE_SYSTEM_VENDOR_NAME "Atmel" )
  set (CMAKE_GENERATOR_PLATFORM AVR8)
else()
  set( CMAKE_SYSTEM_NAME "Generic" )
  set( CMAKE_SYSTEM_VERSION "Generic" )
  set( CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
endif()
  set( CMAKE_SYSTEM_PROCESSOR AVR8 )


find_program( GCC_AR gcc-ar)
# Needed to use the link-time optimization feature
# @see : https://stackoverflow.com/questions/39236917/using-gccs-link-time-optimization-with-static-linked-libraries
set( CMAKE_AR  ${GCC_AR})
set(CMAKE_C_ARCHIVE_CREATE "<CMAKE_AR> qcs <TARGET> <LINK_FLAGS> <OBJECTS>")
set(CMAKE_C_ARCHIVE_FINISH   true)

###########################################################################
# some cmake cross-compile necessities
##########################################################################
if(WIN32)
  get_filename_component(ATMEL_STUDIO_7_INSTALLATION "[HKEY_CURRENT_USER\\Software\\Atmel\\AtmelStudio\\7.0_Config;Install_Dir]" ABSOLUTE CACHE)
  if(${ATMEL_STUDIO_7_INSTALLATION} STREQUAL "")
    message( FATAL_ERROR "Could not find any AtmelStudio7 installation directory in registries." )
  endif()
  find_program( AVR_CC avr-gcc  PATHS ${ATMEL_STUDIO_7_INSTALLATION}  )
  find_program( AVR_CXX avr-g++ PATHS ${ATMEL_STUDIO_7_INSTALLATION} )
  find_program( AVR_OBJCOPY avr-objcopy PATHS ${ATMEL_STUDIO_7_INSTALLATION} )
  find_program( AVR_SIZE_TOOL avr-size  PATHS ${ATMEL_STUDIO_7_INSTALLATION})
  find_program( AVR_OBJDUMP avr-objdump  PATHS ${ATMEL_STUDIO_7_INSTALLATION} )
else()
  find_program( AVR_CC avr-gcc )
  find_program( AVR_CXX avr-g++ )
  find_program( AVR_OBJCOPY avr-objcopy )
  find_program( AVR_SIZE_TOOL avr-size )
  find_program( AVR_OBJDUMP avr-objdump )
endif()

# Verify that toolchain has been resolved correctly
if( NOT DEFINED AVR_CC
    OR NOT DEFINED AVR_CXX
    OR NOT DEFINED AVR_OBJCOPY
    OR NOT DEFINED AVR_SIZE_TOOL
    OR NOT DEFINED AVR_OBJDUMP )
    message(FATAL_ERROR "Could not resolve avr8 toolchain")
endif()

set( CMAKE_C_COMPILER ${AVR_CC} )
set( CMAKE_CXX_COMPILER ${AVR_CXX} )

if (WIN32)
  # Nothing yet to be done
else()
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
endif()


set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# not added automatically, since CMAKE_SYSTEM_NAME is "generic"
if( NOT WIN32)
  set( CMAKE_SYSTEM_INCLUDE_PATH "${CMAKE_FIND_ROOT_PATH}/include" )
  set( CMAKE_SYSTEM_LIBRARY_PATH "${CMAKE_FIND_ROOT_PATH}/lib" )
endif()

##########################################################################
# status messages for generating
##########################################################################
message( STATUS "Set CMAKE_FIND_ROOT_PATH to ${CMAKE_FIND_ROOT_PATH}" )
message( STATUS "Set CMAKE_SYSTEM_INCLUDE_PATH to ${CMAKE_SYSTEM_INCLUDE_PATH}" )
message( STATUS "Set CMAKE_SYSTEM_LIBRARY_PATH to ${CMAKE_SYSTEM_LIBRARY_PATH}" )



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
        (CMAKE_BUILD_TYPE MATCHES RelWithDebInfo) OR
        (CMAKE_BUILD_TYPE MATCHES Debug) OR
        (CMAKE_BUILD_TYPE MATCHES MinSizeRel) ) )
   set(
      CMAKE_BUILD_TYPE Release
      CACHE STRING "Choose cmake build type: Debug Release RelWithDebInfo MinSizeRel"
      FORCE
   )
endif( NOT ( (CMAKE_BUILD_TYPE MATCHES Release) OR
           (CMAKE_BUILD_TYPE MATCHES RelWithDebInfo) OR
           (CMAKE_BUILD_TYPE MATCHES Debug) OR
           (CMAKE_BUILD_TYPE MATCHES MinSizeRel) ) )


####################
# Build configurations with long list of flags
####################

#if(WIN32)
#else(UNIX)
  set(COMPILE_OPTIONS "-funsigned-char \
  -funsigned-bitfields \
  -fpack-struct \
  -fshort-enums \
  -ffunction-sections \
  -fdata-sections \
  -fno-split-wide-types \
  -fno-tree-scev-cprop "
  )

  set(COMPILER_WARNINGS "-Wall \
  -Wextra \
  -Wno-main \
  -Wundef \
  -pedantic \
  -Wstrict-prototypes \
  -Werror \
  -Wfatal-errors ")

  set(COMPILER_LINKER_FORWARD_OPTIONS "-Wl,--gc-sections -Wl,--relax")
  set(FULL_OPTIONS "${COMPILE_OPTIONS} ${COMPILER_WARNINGS} ${COMPILER_LINKER_FORWARD_OPTIONS}")

  set (CMAKE_CXX_FLAGS_RELEASE "-Os -fno-exceptions ${FULL_OPTIONS} " CACHE STRING "Default C++ flags for release" FORCE )
  set (CMAKE_C_FLAGS_RELEASE "-Os ${FULL_OPTIONS}" CACHE STRING "Default C flags for release" FORCE )

  set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -g -gdwarf-2 -fno-exceptions ${FULL_OPTIONS} " CACHE STRING "Default C++ flags for release with debug info" FORCE )
  set (CMAKE_C_FLAGS_RELWITHDEBINFO "-O3 -g -gdwarf-2 ${FULL_OPTIONS} " CACHE STRING "Default C flags for release with debug info" FORCE )

  set( CMAKE_CXX_FLAGS_MINSIZEREL "-Os -mcall-prologues ${FULL_OPTIONS} " CACHE STRING "Default C++ flags for minimum size release" FORCE )
  set( CMAKE_C_FLAGS_MINSIZEREL "-Os -mcall-prologues ${FULL_OPTIONS} " CACHE STRING "Default C flags for minimum size release" FORCE )

  set (CMAKE_CXX_FLAGS_DEBUG "-Og -g3 -gdwarf-2 -fno-exceptions ${FULL_OPTIONS} " CACHE STRING "Default C++ flags for debug configuration" FORCE )
  set (CMAKE_C_FLAGS_DEBUG " -Og -g3 -gdwarf-2 ${FULL_OPTIONS} " CACHE STRING "Default C flags for debug configuration" FORCE )
#endif()

### Function to add an avr executable
function ( add_avr_executable EXECUTABLE_NAME)

    add_executable( ${EXECUTABLE_NAME}.elf
        ${ARGN}
    )

endfunction( add_avr_executable)

### Function to add extra targets to executable
function ( add_extra_avr_targets EXECUTABLE_NAME )

    # Strip binary for upload
    add_custom_target(strip ALL avr-strip ${EXECUTABLE_NAME} -o ${EXECUTABLE_NAME}_stripped DEPENDS ${EXECUTABLE_NAME})

    # Transform binary into hex file, we ignore the eeprom segments in the step
    add_custom_target(hex ALL avr-objcopy -R .eeprom -O ihex ${EXECUTABLE_NAME} ${EXECUTABLE_NAME}.hex DEPENDS strip)

    add_custom_target(size ALL avr-size ${EXECUTABLE_NAME} DEPENDS ${EXECUTABLE_NAME})

endfunction( add_extra_avr_targets )