#--------------------------------------------------------
# The CMakeLists.txt for:                   lib_sock_util
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  SockNinja.cpp
)

SET(HEADERS
  SockNinja.h
)

# Build Library
ADD_LIBRARY(sock_util STATIC ${SRC})
TARGET_LINK_LIBRARIES(sock_util PUBLIC IVP::mbutil)
