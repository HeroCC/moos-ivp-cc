#===========================================================================
# FILE:  moos-ivp-cc/CMakeLists.txt
#===========================================================================

CMAKE_MINIMUM_REQUIRED(VERSION 3.7)
PROJECT( IVP_CC )

#=============================================================================
# Set the output directories for the binary and library files
#=============================================================================

GET_FILENAME_COMPONENT(IVP_CC_BIN_DIR "${CMAKE_SOURCE_DIR}/bin"  ABSOLUTE )
GET_FILENAME_COMPONENT(IVP_CC_LIB_DIR "${CMAKE_SOURCE_DIR}/lib"  ABSOLUTE )

SET( LIBRARY_OUTPUT_PATH      "${IVP_CC_LIB_DIR}" CACHE PATH "" )
SET( ARCHIVE_OUTPUT_DIRECTORY "${IVP_CC_LIB_DIR}" CACHE PATH "" )
SET( LIBRARY_OUTPUT_DIRECTORY "${IVP_CC_LIB_DIR}" CACHE PATH "" )

SET( EXECUTABLE_OUTPUT_PATH   "${IVP_CC_BIN_DIR}" CACHE PATH "" )
SET( RUNTIME_OUTPUT_DIRECTORY "${IVP_CC_BIN_DIR}"  CACHE PATH "" )

#=============================================================================
# Find MOOS and IvP packages
#=============================================================================
find_package(MOOS 10.0 CONFIG REQUIRED)
find_package(MOOSGeodesy CONFIG REQUIRED)
find_package(IVP CONFIG REQUIRED)

#=============================================================================
# Specify Compiler Flags
#=============================================================================
SET(WALL_ON ON CACHE BOOL "tell me about all compiler warnings (-Wall)")

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
   #---------------------------------------------
   # Windows Compiler Flags
   #---------------------------------------------
   IF(WALL_ON)
      SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
   ENDIF(WALL_ON)
elseif()
   #---------------------------------------------
   # Linux and Apple Compiler Flags
   #---------------------------------------------
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
   IF(WALL_ON)
      SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
   ENDIF(WALL_ON)
ENDIF()

# Force -fPIC because gcc complains when we don't use it with x86_64 code.
# Note sure why: -fPIC should only be needed for shared objects, and
# AFAIK, CMake gets that right when building shared objects. -CJC
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Help IDEs along by generating compile commands file
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Don't use compiler features -- hopefully resolve hard-to-detect issues
SET(CMAKE_CXX_EXTENSIONS OFF)

#=============================================================================
# Options
#=============================================================================

option(ENABLE_GAZEBO "Enable building Gazebo related functionality" OFF)

#=============================================================================
# Add Subdirectories
#=============================================================================
ADD_SUBDIRECTORY( src )
