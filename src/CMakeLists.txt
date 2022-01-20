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

if (Boost_VERSION LESS_EQUAL 1.54)
# pWebSocketServer doesn't build on newer versions of Boost, so disable it for now
ADD_SUBDIRECTORY(pWebSocketServer)
endif()

# Copied from moos-ivp-pavlab (r246)
# I don't want to depend on the entire tree, but this is a useful library
ADD_SUBDIRECTORY(lib_sock_utils)

#ADD_SUBDIRECTORY(iMumbleClient) # Moved to Aquaticus tree
ADD_SUBDIRECTORY(uRaspiMon)

# POC, unused
#ADD_SUBDIRECTORY(uFrontNMEABridge)
#ADD_SUBDIRECTORY(uBackNMEABridge)

# SeeByte Project
ADD_SUBDIRECTORY(iNeptune)
#ADD_SUBDIRECTORY(pNeptuneSim) # unused
#ADD_SUBDIRECTORY(iDriveDDS)

# Ignition Gazebo Simulator Plugin
ADD_SUBDIRECTORY(iGazebo)

##############################################################################
#                           END of CMakeLists.txt
##############################################################################

