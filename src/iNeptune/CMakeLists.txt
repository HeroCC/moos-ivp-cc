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
   MOOS::MOOS
   MOOSGeodesy::MOOSGeodesy
   sock_util
   IVP::contacts
   IVP::geometry
   IVP::apputil
   IVP::mbutil
   m
   pthread)

set_property(TARGET iNeptune PROPERTY CXX_STANDARD 11)
