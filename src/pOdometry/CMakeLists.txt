#--------------------------------------------------------
# The CMakeLists.txt for:                       pOdometry
# Author(s):                              Conlan Cesar
#--------------------------------------------------------

SET(SRC
  Odometry.cpp
  Odometry_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pOdometry ${SRC})

TARGET_LINK_LIBRARIES(pOdometry
   ${MOOS_LIBRARIES}
   mbutil
   m
   pthread)

