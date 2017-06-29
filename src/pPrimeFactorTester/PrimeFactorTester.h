/************************************************************/
/*    NAME: Conlan Cesar                                              */
/*    ORGN: MIT                                             */
/*    FILE: PrimeFactorTester.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef PrimeFactorTester_HEADER
#define PrimeFactorTester_HEADER

#include "MOOS/libMOOS/MOOSLib.h"

class PrimeFactorTester : public CMOOSApp
{
 public:
   PrimeFactorTester();
   ~PrimeFactorTester();

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();
   bool ValidatePrimeString(std::string msg);

 protected:
   void RegisterVariables();

 private: // Configuration variables

 private: // State variables
};

#endif 
