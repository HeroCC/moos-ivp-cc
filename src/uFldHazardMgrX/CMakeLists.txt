#--------------------------------------------------------
# The CMakeLists.txt for:                       uFldHazardMgr
# Author(s):                              
#--------------------------------------------------------

SET(SRC
  HazardMgrX.cpp
  HazardMgrX_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(uFldHazardMgrX ${SRC})

TARGET_LINK_LIBRARIES(uFldHazardMgrX
   MOOS::MOOS
   ufld_hazards
   IVP::geometry
   IVP::apputil
   IVP::mbutil
   ${SYSTEM_LIBS}
)

