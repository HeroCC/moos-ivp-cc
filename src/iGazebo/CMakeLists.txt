# Find Qt5, a dependency of Ignition, thanks https://github.com/Homebrew/homebrew-core/issues/8392#issuecomment-325226494
if(APPLE AND EXISTS /usr/local/opt/qt5)
	# Homebrew installs Qt5 (up to at least 5.9.1) in
	# /usr/local/qt5, ensure it can be found by CMake since
	# it is not in the default /usr/local prefix.
	list(APPEND CMAKE_PREFIX_PATH "/usr/local/opt/qt5")
endif()

find_package(ignition-transport11 QUIET REQUIRED OPTIONAL_COMPONENTS log)
set(IGN_TRANSPORT_VER ${ignition-transport11_VERSION_MAJOR})

find_package(sdformat12 REQUIRED)
set(SDF_VER ${sdformat12_VERSION_MAJOR})

find_package(ignition-msgs8 REQUIRED)
set(IGN_MSGS_VER ${ignition-msgs8_VERSION_MAJOR})

find_package(ignition-common4 REQUIRED)
set(IGN_COMMON_VER ${ignition-common4_VERSION_MAJOR})

# Add sources for each plugin to be registered.
add_executable(iGazebo iGazebo.cpp main.cpp iGazebo_Info.cpp)
target_link_libraries(iGazebo
  ignition-transport${IGN_TRANSPORT_VER}::core
  sdformat${SDF_VER}::sdformat${SDF_VER}
  ignition-msgs${IGN_MSGS_VER}
  ignition-common${IGN_COMMON_VER}::ignition-common${IGN_COMMON_VER}
)

# MOOS dependencies
target_link_libraries(iGazebo
  ${MOOS_LIBRARIES}
  ${MOOSGeodesy_LIBRARIES}
  apputil
  mbutil
  m
  pthread
)

# Use C++17
set_property(TARGET iGazebo PROPERTY CXX_STANDARD 17)

# useful https://github.com/ignitionrobotics/ign-gazebo/tree/ign-gazebo6/examples/standalone/keyboard
# https://ignitionrobotics.org/api/gazebo/5.0/underwater_vehicles.html
# https://ignitionrobotics.org/api/gazebo/6.0/createsystemplugins.html
# https://github.com/osrf/mbzirc/blob/main/mbzirc_ign/CMakeLists.txt
