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

