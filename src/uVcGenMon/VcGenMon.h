/****************************************************************/
/*   NAME: Conlan Cesar                                         */
/*   ORGN: MIT Cambridge MA                                     */
/*   FILE: VCGenMon.h                                           */
/*   DATE: Summer 2019                                          */
/****************************************************************/

#ifndef VcGenMon_HEADER
#define VcGenMon_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "VCGenCmd.h"

class VcGenMon : public AppCastingMOOSApp
{
 public:
   VcGenMon();
   ~VcGenMon();

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload 
   bool buildReport();

 protected:
   void registerVariables();
   VCGenCmd vcCmd;

 private: // Configuration variables
    float temperatureWarnThreshold = 70;

 private: // State variables
    bool reportedVcgenFailure = false;
    bool reportedTemperatureThreshSurpassed = false;
};

#endif 
