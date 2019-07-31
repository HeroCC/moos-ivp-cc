#--------------------------------------------------------
# The CMakeLists.txt for:                       uVcGenMon
# Author(s):                              
#--------------------------------------------------------

SET(SRC
  VcGenMon.cpp
  VcGenMon_Info.cpp
  main.cpp
        VCGenCmd.cpp VCGenCmd.h)

ADD_EXECUTABLE(uVcGenMon ${SRC})

TARGET_LINK_LIBRARIES(uVcGenMon
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   m
   pthread)

set_property(TARGET uVcGenMon PROPERTY CXX_STANDARD 11)
