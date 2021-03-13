# PiCode Library
# https://github.com/latchdevel/PiCode
# Copyright (c) 2021 Jorge Rivera. All right reserved.
# License GNU Lesser General Public License v3.0.

cmake_minimum_required(VERSION 3.1)

project(picode)

# Set C/C++ Standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

if(NOT BUILD_COMPILER)
  # Set complier identification
  SET(BUILD_COMPILER "${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}" )
  MESSAGE( STATUS "Compiler: " ${BUILD_COMPILER} )
endif()

if(NOT CMAKE_BUILD_TYPE)
  # Set default build type to "release" set -O3 -DNDEBUG
  set(DEFAULT_BUILD_TYPE "release")

  # Check if set and valid CMAKE_BUILD_TYPE var
  if(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE ${DEFAULT_BUILD_TYPE})
    MESSAGE( STATUS "Build type set to default: " ${CMAKE_BUILD_TYPE} )
  else()
    STRING( TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE )
    if((CMAKE_BUILD_TYPE STREQUAL "debug") OR (CMAKE_BUILD_TYPE STREQUAL "release"))
      MESSAGE( STATUS "Build type set to: " ${CMAKE_BUILD_TYPE} )
    else()
    MESSAGE( FATAL_ERROR "If set CMAKE_BUILD_TYPE it must be Release or Debug")
    endif()
  endif()
endif()

# Setting build type to "debug" add only -g 
if(CMAKE_BUILD_TYPE STREQUAL "debug")
  set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -DDEBUG")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG")
endif()

# Set C/C++ flags
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -Wall -Wextra -Wno-unused-parameter")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter -Wconversion -Woverloaded-virtual")

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

# Add static library libpicode.a
add_library( ${PROJECT_NAME} STATIC $<TARGET_OBJECTS:${PROJECT_NAME}-obj> )

# Add install targets
install(TARGETS ${PROJECT_NAME} DESTINATION lib)
install(TARGETS ${PROJECT_NAME}-dynamic DESTINATION lib)

# If no has parent directory, add uninstall targets
get_directory_property(hasParent PARENT_DIRECTORY)
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
