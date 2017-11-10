#--------------------------------------------------------
# The CMakeLists.txt for:                       pWebSocketServer
# Author(s):                              
#--------------------------------------------------------


SET(SRC
  WebSocketServer.cpp
  WebSocketServer_Info.cpp
  main.cpp
        WebSocketClient.cpp WebSocketClient.h)

# TODO This is all one big hack copied from the other CMake file, make it simpler (compile the library seperate)
# https://codedump.io/share/L3qcmOuUZDDE/1/how-to-create-a-cmake-header-only-library-that-depends-on-external-header-files

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wsign-conversion")
include_directories(Simple-WebSocket-Server)

# Your-external "mylib", add GLOBAL if the imported library is located in directories above the current.
#add_library( Simple-WebSocket-Server INTERFACE )
#add_dependencies(Simple-WebSocket-Server lib/Simple-WebSocket-Server)
#target_include_directories(Simple-WebSocket-Server SYSTEM INTERFACE lib/Simple-WebSocket-Server)
# You can define two import-locations: one for debug and one for release.
#set_target_properties( Simple-WebSocket-Server PROPERTIES IMPORTED_LOCATION lib/Simple-WebSocket-Server)
#target_link_libraries(Simple-WebSocket-Server INTERFACE)

set(BOOST_COMPONENTS system coroutine context thread)
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.9)
        set(BOOST_COMPONENTS ${BOOST_COMPONENTS} regex)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_BOOST_REGEX")
    endif()
endif()
find_package(Boost 1.54.0 COMPONENTS ${BOOST_COMPONENTS} REQUIRED)
include_directories(SYSTEM ${Boost_INCLUDE_DIR})

if(APPLE)
    set(OPENSSL_ROOT_DIR "/usr/local/opt/openssl")
endif()
find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

find_package(Threads REQUIRED)

ADD_EXECUTABLE(pWebSocketServer ${SRC})

TARGET_LINK_LIBRARIES(pWebSocketServer
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   m
   pthread)

target_link_libraries(pWebSocketServer ${Boost_LIBRARIES})
target_link_libraries(pWebSocketServer ${OPENSSL_CRYPTO_LIBRARY})
target_link_libraries(pWebSocketServer ${CMAKE_THREAD_LIBS_INIT})

