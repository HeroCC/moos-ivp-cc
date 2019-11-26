#--------------------------------------------------------
# The CMakeLists.txt for:                       iMumbleClient
# Author(s):                              
#--------------------------------------------------------

add_subdirectory(mumlib)

SET(SRC
  MumbleClient.cpp
  MumbleClient_Info.cpp
  main.cpp
        MumbleCallbackHandler.cpp MumbleCallbackHandler.h SimpleRingBuffer.h)

ADD_EXECUTABLE(iMumbleClient ${SRC})

target_compile_definitions(iMumbleClient PRIVATE DUMP_CORE)

TARGET_LINK_LIBRARIES(iMumbleClient
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   m
   pthread
        mumlib
        portaudio)

set_property(TARGET iMumbleClient PROPERTY CXX_STANDARD 11)