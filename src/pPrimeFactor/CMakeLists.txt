#--------------------------------------------------------
# The CMakeLists.txt for:                       pPrimeFactor
# Author(s):                              Conlan Cesar
#--------------------------------------------------------

SET(SRC
  PrimeFactor.cpp
  PrimeFactorEntry.cpp
  PrimeFactor_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pPrimeFactor ${SRC})

TARGET_LINK_LIBRARIES(pPrimeFactor
   ${MOOS_LIBRARIES}
   mbutil
   m
   pthread)

