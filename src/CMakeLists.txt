##############################################################################
# FILE:        moos-ivp-extend/src/CMakeLists.txt
# DATE:        2010/09/07
# DESCRIPTION: CMakeLists.txt file for the moos-ivp-extend source directory
##############################################################################


#============================================================================
# Add the libraries in the current directory to the include path
#============================================================================
FILE(GLOB LOCAL_LIBRARY_DIRS ./lib_*)
INCLUDE_DIRECTORIES(${LOCAL_LIBRARY_DIRS})

#============================================================================
# List the subdirectories to build...
#============================================================================

#SET(BUILD_LABS please) # Uncomment this to build the labs

IF(BUILD_LABS)
ADD_SUBDIRECTORY(lib_behaviors-test)
ADD_SUBDIRECTORY(pXRelayTest)
ADD_SUBDIRECTORY(pExampleApp)
ADD_SUBDIRECTORY(pPrimeFactor)
ADD_SUBDIRECTORY(pPrimeFactorTester)
#ADD_SUBDIRECTORY(pFoobar)
ADD_SUBDIRECTORY(pOdometry)
ADD_SUBDIRECTORY(pPointAssign)
ADD_SUBDIRECTORY(pGenPath)
ADD_SUBDIRECTORY(uFldHazardMgrX)
ENDIF()

ADD_SUBDIRECTORY(pWebSocketServer)
ADD_SUBDIRECTORY(iMumbleClient)
ADD_SUBDIRECTORY(uRaspiMon)

##############################################################################
#                           END of CMakeLists.txt
##############################################################################

