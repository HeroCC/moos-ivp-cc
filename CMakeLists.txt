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
# Find MOOS
#=============================================================================
find_package(MOOS 10.0 REQUIRED)
INCLUDE_DIRECTORIES(${MOOS_INCLUDE_DIRS})

#=============================================================================
# FINDING MOOSGeodesy' HEADERS AND LIBRARIES...
#=============================================================================
find_package(MOOSGeodesy REQUIRED)
include_directories(${MOOSGeodesy_INCLUDE_DIRS})
link_directories(${MOOSGeodesy_LIBRARY_PATH})

#=============================================================================
# Find the "moos-ivp" base directory
#=============================================================================

# Search for the moos-ivp folder
find_path( MOOSIVP_SOURCE_TREE_BASE
           NAMES build-ivp.sh build-moos.sh configure-ivp.sh
           PATHS "../moos-ivp" "../../moos-ivp" "../../moos-ivp/trunk/" "../moos-ivp/trunk/" "${HOME}/moos-ivp/trunk/" "${HOME}/moos-ivp/"
           DOC "Base directory of the MOOS-IvP source tree"
           NO_DEFAULT_PATH
)

if (NOT MOOSIVP_SOURCE_TREE_BASE)
    message("Please set MOOSIVP_SOURCE_TREE_BASE to  ")
    message("the location of the \"moos-ivp\" folder ")
    return()
endif()

#=============================================================================
# Specify where to find IvP's headers and libraries...
#=============================================================================

FILE(GLOB IVP_INCLUDE_DIRS ${MOOSIVP_SOURCE_TREE_BASE}/ivp/src/lib_* )
INCLUDE_DIRECTORIES(${IVP_INCLUDE_DIRS})

FILE(GLOB IVP_LIBRARY_DIRS ${MOOSIVP_SOURCE_TREE_BASE}/lib )
LINK_DIRECTORIES(${IVP_LIBRARY_DIRS})

if (APPLE)
   find_program(MACPORTS_BIN port)
   if (MACPORTS_BIN)
      include_directories(/opt/local/include)
      link_directories(/opt/local/lib) # Include the default MacPorts directory
   endif()

   find_program(MAC_HBREW_BIN brew)
   if (MAC_HBREW_BIN)
      execute_process(COMMAND ${MAC_HBREW_BIN} "--prefix" OUTPUT_VARIABLE BREW_PREFIX OUTPUT_STRIP_TRAILING_WHITESPACE)
      list(INSERT CMAKE_PREFIX_PATH 0 ${BREW_PREFIX}) # Homebrew itself
      list(INSERT CMAKE_PREFIX_PATH 0 ${BREW_PREFIX}/opt/openssl) # Prefer Homebrew OpenSSL
      link_directories(${BREW_PREFIX}/lib)
    endif()
endif()

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
