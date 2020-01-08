/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: BackNMEABridge.h                                          */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#ifndef BackNMEABridge_HEADER
#define BackNMEABridge_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

class BackNMEABridge : public AppCastingMOOSApp
{
 public:
   BackNMEABridge();
   ~BackNMEABridge();

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload 
   bool buildReport();

 protected:
   void registerVariables();

 private: // Configuration variables

 private: // State variables
};

#endif 
