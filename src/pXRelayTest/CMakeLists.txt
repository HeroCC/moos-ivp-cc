#--------------------------------------------------------
# The CMakeLists.txt for:                     pXRelayTest
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

# Set System Specific Libraries
if (${WIN32})
  # Windows Libraries
  SET(SYSTEM_LIBS
     wsock32 )
else (${WIN32})
  # Linux and Apple Libraries
  SET(SYSTEM_LIBS
      m
      pthread )
endif (${WIN32})


SET(SRC
   Relayer.cpp  
   Relayer_Info.cpp  
   main.cpp
)  

ADD_EXECUTABLE(pXRelayTest ${SRC})

TARGET_LINK_LIBRARIES(pXRelayTest
   ${MOOS_LIBRARIES}
   mbutil
   ${SYSTEM_LIBS} )
