# PiCode Library
# https://github.com/latchdevel/PiCode
# Copyright (c) 2021 Jorge Rivera. All right reserved.
# License GNU Lesser General Public License v3.0.

cmake_minimum_required(VERSION 3.1)

project(picode)

# Set version number for shared libraries and executables
set(CU_VERSION 1.1) # current version
set(SO_VERSION 1.1) # compatibility version

# Set C/C++ Standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

if(NOT BUILD_COMPILER)
  # Set complier identification
  SET(BUILD_COMPILER "${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}" )
  MESSAGE( STATUS "Compiler: " ${BUILD_COMPILER} )
endif()

# Check if has parent directory
get_directory_property(hasParent PARENT_DIRECTORY)

# Check if set CMAKE_BUILD_TYPE var
if(NOT CMAKE_BUILD_TYPE)
  # Set default build type to "release" set -O3 -DNDEBUG
  set(DEFAULT_BUILD_TYPE "release")
  SET(CMAKE_BUILD_TYPE ${DEFAULT_BUILD_TYPE})
  MESSAGE( STATUS "Build type set to default: " ${CMAKE_BUILD_TYPE} )
else()
  # Check if set and valid CMAKE_BUILD_TYPE var
  STRING( TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE )
  if((CMAKE_BUILD_TYPE STREQUAL "debug") OR (CMAKE_BUILD_TYPE STREQUAL "release"))
    # If no has parent directory show message
    if(NOT hasParent)
      MESSAGE( STATUS "Build type set to: " ${CMAKE_BUILD_TYPE} )
    endif()
  else()
    MESSAGE( FATAL_ERROR "If set CMAKE_BUILD_TYPE it must be 'release' or 'debug'")
  endif()
endif()

# Setting build type to "debug" add only -g 
if(CMAKE_BUILD_TYPE STREQUAL "debug") 
  set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -DDEBUG")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG")
endif()

# Set C++ flags
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -Wall -Wextra -Wno-unused-parameter")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter -Wconversion -Woverloaded-virtual -Wsign-conversion")
elseif(MSVC)
  set(MSVC_DISABLED_WARNINGS_LIST
    "C4996" # warning C4996: 'may be unsafe/disable deprecation'
            # To disable deprecation, use _CRT_SECURE_NO_WARNINGS.
  # "C4244" # warning C4244: '=': conversion from '__int64' to 'int', possible loss of data
  # "C4267" # warning C4267: '=': conversion from 'size_t' to 'int', possible loss of data
  # "C4305" # warning C4305: '=': truncation from 'int' to 'uint16_t'
    "C5105" # warning C5105: macro expansion producing 'defined' has undefined behavior
    "C4201" # warning C4201: nonstandard extension used: nameless struct/union
    "C4473" # warning C4473: 'printf' : not enough arguments passed for format string
    "C4100" # warning C4100: unreferenced formal parameter
    "C4706" # warning C4706: assignment within conditional expression
  )
  string(REPLACE "C" " -wd" MSVC_DISABLED_WARNINGS_STR ${MSVC_DISABLED_WARNINGS_LIST})
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -W4 ${MSVC_DISABLED_WARNINGS_STR}")
  set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -Wall -W4 ${MSVC_DISABLED_WARNINGS_STR}")
endif()

# Add source directory for sources
AUX_SOURCE_DIRECTORY( src/ ${PROJECT_NAME}_SRC ) 

# Add library aditional sources 
AUX_SOURCE_DIRECTORY( libs/pilight/libs/pilight/core/ CORE ) 
AUX_SOURCE_DIRECTORY( libs/pilight/libs/pilight/protocols/ PROTOCOL ) 
AUX_SOURCE_DIRECTORY( libs/pilight/libs/pilight/protocols/433.92/ PROTOCOLS ) 

# Compile as object library
add_library( 
                ${PROJECT_NAME}-obj OBJECT
                ${${PROJECT_NAME}_SRC}
                ${CORE}
                ${PROTOCOL} 
                ${PROTOCOLS}
)

# Shared libraries need flag -fPIC
set_property(TARGET ${PROJECT_NAME}-obj PROPERTY POSITION_INDEPENDENT_CODE 1)

# Shared library built from the same object files 
# File extension OS depends, like: libpicode.so or libpicode.dylib or libpicode.dll
add_library( ${PROJECT_NAME}-dynamic SHARED $<TARGET_OBJECTS:${PROJECT_NAME}-obj> )
set_target_properties( ${PROJECT_NAME}-dynamic PROPERTIES OUTPUT_NAME ${PROJECT_NAME} )

# Set version numbers for the versioned shared libraries target.
# For shared libraries and executables on Windows and Mach-O systems 
# the SOVERSION property corresponds to the compatibility version 
# and VERSION corresponds to the current version
# 
# Note that SOVERSION will still be used to form the install_name and 
# both SOVERSION and VERSION may also affect the file and symlink names.
# Use the NAMELINK_SKIP option of the install command to prevent the 
# generation of the versionless library name symbolic link to the 
# versioned library file.
set_target_properties( ${PROJECT_NAME}-dynamic PROPERTIES
		SOVERSION ${SO_VERSION}
		VERSION   ${CU_VERSION}
)

# Add static library libpicode.a
add_library( ${PROJECT_NAME} STATIC $<TARGET_OBJECTS:${PROJECT_NAME}-obj> )

# Add install targets
install(TARGETS ${PROJECT_NAME} DESTINATION lib)
install(TARGETS ${PROJECT_NAME}-dynamic DESTINATION lib)

# If no has parent directory, add uninstall targets
if(NOT hasParent)
  MESSAGE(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
  add_custom_target( uninstall
      "${CMAKE_COMMAND}" -P "${CMAKE_SOURCE_DIR}/uninstall.cmake"
  )
endif()

# Add picode_example source file, link static, no build as default 
add_executable( picode_example picode_example.cpp )
target_link_libraries( picode_example PRIVATE ${PROJECT_NAME} )
set_target_properties( picode_example PROPERTIES EXCLUDE_FROM_ALL TRUE )
