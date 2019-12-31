#--------------------------------------------------------
# The CMakeLists.txt for:                       uFrontNMEABridge
# Author(s):                              
#--------------------------------------------------------

SET(SRC
  FrontNMEABridge.cpp
  FrontNMEABridge_Info.cpp
  main.cpp
  Socket.cpp Socket.h)

ADD_EXECUTABLE(uFrontNMEABridge ${SRC})

TARGET_LINK_LIBRARIES(uFrontNMEABridge
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   m
   pthread)

set_property(TARGET uFrontNMEABridge PROPERTY CXX_STANDARD 11)

