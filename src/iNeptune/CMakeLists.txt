#--------------------------------------------------------
# The CMakeLists.txt for:                       iNeptune
# Author(s):                              
#--------------------------------------------------------

SET(SRC
  Neptune.cpp
  Neptune_Info.cpp
  main.cpp
  Socket.cpp)

ADD_EXECUTABLE(iNeptune ${SRC})

TARGET_LINK_LIBRARIES(iNeptune
   ${MOOS_LIBRARIES}
   ${MOOSGeodesy_LIBRARIES}
   apputil
   mbutil
   m
   pthread)

set_property(TARGET iNeptune PROPERTY CXX_STANDARD 11)

