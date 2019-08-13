/****************************************************************/
/*   NAME: Conlan Cesar                                         */
/*   ORGN: MIT Cambridge MA                                     */
/*   FILE: RaspiMon.h                                           */
/*   DATE: Summer 2019                                          */
/****************************************************************/

#ifndef RaspiMon_HEADER
#define RaspiMon_HEADER

#include <deque>
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "VCGenCmd.h"

class RaspiMon : public AppCastingMOOSApp
{
 public:
   RaspiMon();
   ~RaspiMon();

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
    float temperatureWarnThreshold;

 private: // State variables
    bool reportedVcgenFailure;
    bool reportedTemperatureThreshSurpassed;
    std::deque<float> tempHistory;
};

#endif 
