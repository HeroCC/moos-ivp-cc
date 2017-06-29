#--------------------------------------------------------
# The CMakeLists.txt for:                     pExampleApp
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
   main.cpp  
)  

ADD_EXECUTABLE(pExampleApp ${SRC})

TARGET_LINK_LIBRARIES(pExampleApp
   ${MOOS_LIBRARIES}
   ${SYSTEM_LIBS} )
