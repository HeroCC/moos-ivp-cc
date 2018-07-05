#--------------------------------------------------------
# The CMakeLists.txt for:                       iMumbleClient
# Author(s):                              
#--------------------------------------------------------

add_subdirectory(mumlib)

SET(SRC
  MumbleClient.cpp
  MumbleClient_Info.cpp
  main.cpp
        MumbleCallbackHandler.cpp)

ADD_EXECUTABLE(iMumbleClient ${SRC})

TARGET_LINK_LIBRARIES(iMumbleClient
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   m
   pthread
        mumlib
        portaudio)

