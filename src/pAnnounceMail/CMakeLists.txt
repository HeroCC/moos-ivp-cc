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
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   m
   pthread)

