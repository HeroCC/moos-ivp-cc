#--------------------------------------------------------
# The CMakeLists.txt for:                       uBackNMEABridge
# Author(s):                              
#--------------------------------------------------------

SET(SRC
  BackNMEABridge.cpp
  BackNMEABridge_Info.cpp
  main.cpp
        Socket.h Socket.cpp
)


ADD_EXECUTABLE(uBackNMEABridge ${SRC})

TARGET_LINK_LIBRARIES(uBackNMEABridge
   MOOSGeodesy::MOOSGeodesy
   MOOS::MOOS
   IVP::apputil
   IVP::mbutil
   m
   pthread
   #IVP::geometry
   )

set_property(TARGET uBackNMEABridge PROPERTY CXX_STANDARD 11)
