#--------------------------------------------------------
# The CMakeLists.txt for:                       pGenPath
# Author(s):                              Conlan Cesar
#--------------------------------------------------------

SET(SRC
  GenPath.cpp
  GenPath_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pGenPath ${SRC})

TARGET_LINK_LIBRARIES(pGenPath
   MOOS::MOOS
   IVP::mbutil
   m
   IVP::geometry
   pthread)

