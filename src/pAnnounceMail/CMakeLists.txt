#--------------------------------------------------------
# The CMakeLists.txt for:                       pAnnounceMail
# Author(s):                              
#--------------------------------------------------------

SET(SRC
  AnnounceMail.cpp
  AnnounceMail_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pAnnounceMail ${SRC})

TARGET_LINK_LIBRARIES(pAnnounceMail
   MOOS::MOOS
   IVP::apputil
   IVP::mbutil
   m
   pthread)

