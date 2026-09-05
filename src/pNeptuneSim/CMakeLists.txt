#--------------------------------------------------------
# The CMakeLists.txt for:                       pNeptuneSim
# Author(s):                              
#--------------------------------------------------------

SET(SRC
  NeptuneSim.cpp
  NeptuneSim_Info.cpp
  main.cpp
  Socket.cpp NMEAUtils.cpp)

ADD_EXECUTABLE(pNeptuneSim ${SRC})

TARGET_LINK_LIBRARIES(pNeptuneSim
   MOOSGeodesy::MOOSGeodesy
   MOOS::MOOS
   IVP::apputil
   IVP::mbutil
   m
   pthread)

set_property(TARGET pNeptuneSim PROPERTY CXX_STANDARD 11)

