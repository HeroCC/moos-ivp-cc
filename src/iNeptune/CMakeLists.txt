#--------------------------------------------------------
# The CMakeLists.txt for:                       iNeptune
# Author(s):                              
#--------------------------------------------------------

SET(SRC
  Neptune.cpp
  Neptune_Info.cpp
  main.cpp
  NMEAUtils.cpp
  LooseNinja.cpp
)

ADD_EXECUTABLE(iNeptune ${SRC})

TARGET_LINK_LIBRARIES(iNeptune
   ${MOOS_LIBRARIES}
   ${MOOSGeodesy_LIBRARIES}
   sock_util
   contacts
   geometry
   apputil
   mbutil
   m
   pthread)

set_property(TARGET iNeptune PROPERTY CXX_STANDARD 11)
