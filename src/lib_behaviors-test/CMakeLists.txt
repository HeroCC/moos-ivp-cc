#--------------------------------------------------------
# The CMakeLists.txt for:              lib_behaviors-test
# Author(s):
#--------------------------------------------------------

# Set System Specific Libraries
if (${WIN32})
  # Windows Libraries
  SET(SYSTEM_LIBS
      )
else (${WIN32})
  # Linux and Apple Libraries
  SET(SYSTEM_LIBS
      m )
endif (${WIN32})


MACRO(ADD_BHV BHV_NAME)
  ADD_LIBRARY(${BHV_NAME} SHARED ${BHV_NAME}.cpp)
  TARGET_LINK_LIBRARIES(${BHV_NAME}
    IVP::helmivp
    IVP::behaviors
    IVP::ivpbuild
    IVP::logic
    IVP::ivpcore
    IVP::bhvutil
    IVP::mbutil
    IVP::geometry
    ${SYSTEM_LIBS} )
ENDMACRO(ADD_BHV)


#--------------------------------------------------------
#                                      BHV_SimpleWaypoint
#--------------------------------------------------------
ADD_LIBRARY(BHV_SimpleWaypoint SHARED
   BHV_SimpleWaypoint.cpp AOF_SimpleWaypoint.cpp)
TARGET_LINK_LIBRARIES(BHV_SimpleWaypoint
   IVP::helmivp
   IVP::behaviors
   IVP::ivpbuild
   IVP::logic
   IVP::ivpcore
   IVP::bhvutil
   IVP::mbutil
   IVP::geometry
   ${SYSTEM_LIBS} )

 #--------------------------------------------------------
 #                                      BHV_Pulse
 #--------------------------------------------------------
 ADD_LIBRARY(BHV_Pulse SHARED
    BHV_Pulse.cpp)
 TARGET_LINK_LIBRARIES(BHV_Pulse
    IVP::helmivp
    IVP::behaviors
    IVP::ivpbuild
    IVP::logic
    IVP::ivpcore
    IVP::bhvutil
    IVP::mbutil
    IVP::geometry
    ${SYSTEM_LIBS} )

#--------------------------------------------------------
#                                      BHV_ZigLeg
#--------------------------------------------------------
ADD_LIBRARY(BHV_ZigLeg SHARED
        BHV_ZigLeg.cpp)
TARGET_LINK_LIBRARIES(BHV_ZigLeg
        IVP::helmivp
        IVP::behaviors
        IVP::ivpbuild
        IVP::logic
        IVP::ivpcore
        IVP::bhvutil
        IVP::mbutil
        IVP::geometry
        ${SYSTEM_LIBS} )