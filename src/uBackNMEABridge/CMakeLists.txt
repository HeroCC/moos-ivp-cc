#--------------------------------------------------------
# The CMakeLists.txt for:                       uBackNMEABridge
# Author(s):                              
#--------------------------------------------------------

SET(SRC
  BackNMEABridge.cpp
  BackNMEABridge_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(uBackNMEABridge ${SRC})

TARGET_LINK_LIBRARIES(uBackNMEABridge
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   m
   pthread)

