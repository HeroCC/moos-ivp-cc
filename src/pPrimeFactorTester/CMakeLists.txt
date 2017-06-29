#--------------------------------------------------------
# The CMakeLists.txt for:                       pPrimeFactorTester
# Author(s):                              Conlan Cesar
#--------------------------------------------------------

SET(SRC
  PrimeFactorTester.cpp
  PrimeFactorTester_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pPrimeFactorTester ${SRC})

TARGET_LINK_LIBRARIES(pPrimeFactorTester
   ${MOOS_LIBRARIES}
   mbutil
   m
   pthread)

