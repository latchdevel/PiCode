# PiCode Library
# https://github.com/latchdevel/PiCode
# Copyright (c) 2021 Jorge Rivera. All right reserved.
# License GNU Lesser General Public License v3.0.

cmake_minimum_required(VERSION 3.1)

project(picode)

# Set version number for shared libraries and executables
set(CU_VERSION 1.3) # current version
set(SO_VERSION 1.1) # compatibility version

# Set C/C++ Standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

if(NOT BUILD_COMPILER)
  # Set complier identification
  SET(BUILD_COMPILER "${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}" )
  MESSAGE( STATUS "Compiler: " ${BUILD_COMPILER} )
endif()

# Check for git repository information
IF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/.git)
  FIND_PACKAGE(Git)
  IF(GIT_FOUND)
    EXECUTE_PROCESS(
      COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
      OUTPUT_VARIABLE "BUILD_VERSION"
      ERROR_QUIET
      OUTPUT_STRIP_TRAILING_WHITESPACE)
    SET(BUILD_VERSION "${BUILD_VERSION}" )
    MESSAGE( STATUS "Git commit: ${BUILD_VERSION}")
  ENDIF(GIT_FOUND)
ENDIF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/.git)

# Force re-run CMake to set BUILD_VERSION on any git commit
set_property(
            DIRECTORY
            APPEND
            PROPERTY CMAKE_CONFIGURE_DEPENDS
            .git/index
)

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
  set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -Wall -Wextra -Wno-unused-parameter -fcommon")
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

# If macOS builds a Mach-O universal binary with 2 architectures: x86_64 and arm64 for Apple M processors
if (APPLE) 
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64")
endif()

# Add sources for C++ library
FILE (GLOB ${PROJECT_NAME}_SRC_CPP src/*.cpp )

# Pure C PiCode Library sources
FILE (GLOB ${PROJECT_NAME}_SRC_C src/*.c )

# Add library common sources 
AUX_SOURCE_DIRECTORY( libs/pilight/libs/pilight/core/ CORE ) 
AUX_SOURCE_DIRECTORY( libs/pilight/libs/pilight/protocols/ PROTOCOL ) 
AUX_SOURCE_DIRECTORY( libs/pilight/libs/pilight/protocols/433.92/ PROTOCOLS ) 

# Compile common library objects
add_library( 
                ${PROJECT_NAME}-common OBJECT
                ${CORE}
                ${PROTOCOL} 
                ${PROTOCOLS}
)

# Compile C++ library as object 
add_library( 
                ${PROJECT_NAME}-obj OBJECT
                ${${PROJECT_NAME}_SRC_CPP}
)

# Pure C PiCode Library compile as object
add_library( 
                c${PROJECT_NAME}-obj OBJECT
                ${${PROJECT_NAME}_SRC_C}
)
# If git info available adds to pure C PiCode Library
if(DEFINED BUILD_VERSION)
    target_compile_definitions( c${PROJECT_NAME}-obj PRIVATE BUILD_VERSION=${BUILD_VERSION} )
endif()

# If version number for shared libraries and executables available adds to pure C PiCode Library
if(DEFINED CU_VERSION)
    target_compile_definitions( c${PROJECT_NAME}-obj PRIVATE CU_VERSION=${CU_VERSION} )
endif()

# Shared libraries need flag -fPIC
set_property(TARGET  ${PROJECT_NAME}-common PROPERTY POSITION_INDEPENDENT_CODE 1)
set_property(TARGET  ${PROJECT_NAME}-obj    PROPERTY POSITION_INDEPENDENT_CODE 1)
# Pure C PiCode Library
set_property(TARGET c${PROJECT_NAME}-obj    PROPERTY POSITION_INDEPENDENT_CODE 1)

# Shared library built from the same object files 
add_library( 
                ${PROJECT_NAME}-dynamic SHARED 
                $<TARGET_OBJECTS:${PROJECT_NAME}-obj> 
                $<TARGET_OBJECTS:${PROJECT_NAME}-common> 
)
# C++ PiCode Library v1.2 require the pure C library (dynamic or static) as "c${PROJECT_NAME}-dynamic"
target_link_libraries(${PROJECT_NAME}-dynamic PUBLIC c${PROJECT_NAME}) 

# File extension OS depends, like: libpicode.so or libpicode.dylib or libpicode.dll
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

# Add static library 
# File extension OS depends, like: libpicode.a or libpicode.lib
add_library( 
                ${PROJECT_NAME} STATIC 
                $<TARGET_OBJECTS:${PROJECT_NAME}-obj> 
                $<TARGET_OBJECTS:${PROJECT_NAME}-common> 
)
# C++ PiCode Library v1.2 require the pure C library static as "c${PROJECT_NAME}"
target_link_libraries(${PROJECT_NAME} PUBLIC c${PROJECT_NAME}) 

# Pure C PiCode Library dynamic
# ---------------------------------------------------------------------------------
# Shared library built from the same object files 
add_library( 
              c${PROJECT_NAME}-dynamic SHARED 
              $<TARGET_OBJECTS:c${PROJECT_NAME}-obj> 
              $<TARGET_OBJECTS:${PROJECT_NAME}-common>
)
# File extension OS depends, like: libcpicode.so or libcpicode.dylib or libcpicode.dll
set_target_properties( c${PROJECT_NAME}-dynamic PROPERTIES OUTPUT_NAME c${PROJECT_NAME} )

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
set_target_properties( c${PROJECT_NAME}-dynamic PROPERTIES
		SOVERSION ${SO_VERSION}
		VERSION   ${CU_VERSION}
)

# Pure C PiCode Library static 
# File extension OS depends, like: libcpicode.a or libcpicode.lib
add_library( 
                c${PROJECT_NAME} STATIC 
                $<TARGET_OBJECTS:c${PROJECT_NAME}-obj> 
                $<TARGET_OBJECTS:${PROJECT_NAME}-common>
)

# Add install targets
install(TARGETS ${PROJECT_NAME} DESTINATION lib)
install(TARGETS ${PROJECT_NAME}-dynamic DESTINATION lib)

# Pure C PiCode Library install targets
# ---------------------------------------------------------------------------------
install(TARGETS c${PROJECT_NAME} DESTINATION lib)
install(TARGETS c${PROJECT_NAME}-dynamic DESTINATION lib)

# If no has parent directory, add uninstall targets
if(NOT hasParent)
  MESSAGE(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
  add_custom_target( uninstall
      "${CMAKE_COMMAND}" -P "${CMAKE_SOURCE_DIR}/uninstall.cmake"
  )
endif()

# Examples
# ---------------------------------------------------------------------------------
# Checking for math library 'libm' used when including <math.h> in pilight sources
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a;.lib;.tbd;.so;.dylib;.dll.a")
find_library(MATH_LIBRARY NAMES m )
if(MATH_LIBRARY)
  MESSAGE( STATUS "Math library: " ${MATH_LIBRARY} )
else()
  if(NOT MSVC)
    message(FATAL_ERROR "Cannot find math library 'libm' ")
  else()
    set(MATH_LIBRARY "")
  endif()
endif()

# Add picode_example source file, link static, no build as default 
add_executable( picode_example picode_example.cpp )
target_link_libraries( picode_example PRIVATE ${PROJECT_NAME} ${MATH_LIBRARY} )
set_target_properties( picode_example PROPERTIES EXCLUDE_FROM_ALL TRUE )

# Add executable export symbols for loadable modules to prevent policy CMP0065 warning
# Basic check for BSD based systems, like as FreeBSD, NetBSD, OpenBSD, etc.
string( TOUPPER ${CMAKE_SYSTEM_NAME} CMAKE_SYSTEM_NAME_UPPER )
if(NOT CMAKE_SYSTEM_NAME_UPPER MATCHES "BSD")
  # If not a BSD system, add executable export symbols for loadable modules. Add linker flag "-Wl,--export-dynamic"
  set_target_properties( picode_example PROPERTIES ENABLE_EXPORTS TRUE ) 
endif()

# If git info available adds to picode_example executable as environment var
if(DEFINED BUILD_VERSION)
    target_compile_definitions( picode_example PRIVATE BUILD_VERSION=${BUILD_VERSION} )
endif(DEFINED BUILD_VERSION)

# Add complier identification to picode_example executable as environment var
target_compile_definitions( picode_example PRIVATE BUILD_COMPILER=${BUILD_COMPILER} )

# Pure C PiCode Library example
# ---------------------------------------------------------------------------------
# Add cpicode_example source file, link static, no build as default 
add_executable( cpicode_example cpicode_example.c )
target_link_libraries( cpicode_example PRIVATE c${PROJECT_NAME} ${MATH_LIBRARY} )
set_target_properties( cpicode_example PROPERTIES EXCLUDE_FROM_ALL TRUE )

# Add executable export symbols for loadable modules to prevent policy CMP0065 warning
# Basic check for BSD based systems, like as FreeBSD, NetBSD, OpenBSD, etc.
string( TOUPPER ${CMAKE_SYSTEM_NAME} CMAKE_SYSTEM_NAME_UPPER )
if(NOT CMAKE_SYSTEM_NAME_UPPER MATCHES "BSD")
  # If not a BSD system, add executable export symbols for loadable modules. Add linker flag "-Wl,--export-dynamic"
  set_target_properties( cpicode_example PROPERTIES ENABLE_EXPORTS TRUE ) 
endif()

# If git info available adds to cpicode_example executable as environment var
if(DEFINED BUILD_VERSION)
    target_compile_definitions( cpicode_example PRIVATE BUILD_VERSION=${BUILD_VERSION} )
endif(DEFINED BUILD_VERSION)

# Add complier identification to cpicode_example executable as environment var
target_compile_definitions( cpicode_example PRIVATE BUILD_COMPILER=${BUILD_COMPILER} )
