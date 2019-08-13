#--------------------------------------------------------
# The CMakeLists.txt for:                       uRaspiMon
# Author(s):                              
#--------------------------------------------------------

SET(SRC
        RaspiMon.cpp
        RaspiMon_Info.cpp
  main.cpp
        VCGenCmd.cpp VCGenCmd.h)

ADD_EXECUTABLE(uRaspiMon ${SRC})

TARGET_LINK_LIBRARIES(uRaspiMon
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   m
   pthread)

set_property(TARGET uRaspiMon PROPERTY CXX_STANDARD 11)
