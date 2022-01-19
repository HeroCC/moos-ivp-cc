# Find Qt5, a dependency of Ignition, thanks https://github.com/Homebrew/homebrew-core/issues/8392#issuecomment-325226494
if(APPLE AND EXISTS /usr/local/opt/qt5)
	# Homebrew installs Qt5 (up to at least 5.9.1) in
	# /usr/local/qt5, ensure it can be found by CMake since
	# it is not in the default /usr/local prefix.
	list(APPEND CMAKE_PREFIX_PATH "/usr/local/opt/qt5")
endif()

# Find Ign Gazebo Deps
find_package(ignition-plugin1 REQUIRED COMPONENTS loader register)
set(IGN_PLUGIN_VER ${ignition-plugin1_VERSION_MAJOR})

# Find Ign Gazebo itself
find_package(ignition-gazebo6 REQUIRED)

# Add sources for each plugin to be registered.
add_library(gazebo_moos SHARED gazebo_moos.cc iGazebo.cpp)
target_link_libraries(gazebo_moos
  PRIVATE ignition-plugin${IGN_PLUGIN_VER}::ignition-plugin${IGN_PLUGIN_VER}
  PRIVATE ignition-gazebo6::ignition-gazebo6
)
target_link_libraries(gazebo_moos PRIVATE
  ${MOOS_LIBRARIES}
  ${MOOSGeodesy_LIBRARIES}
  apputil
  mbutil
  m
  pthread
)

# Use C++17
set_property(TARGET gazebo_moos PROPERTY CXX_STANDARD 17)

# useful https://github.com/osrf/gazebo/tree/gazebo11/examples/plugins/model_move
# https://ignitionrobotics.org/api/gazebo/5.0/underwater_vehicles.html
# https://ignitionrobotics.org/api/gazebo/6.0/createsystemplugins.html
# https://github.com/osrf/mbzirc/blob/main/mbzirc_ign/CMakeLists.txt
