/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: BackNMEABridge.h                                          */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#ifndef BackNMEABridge_HEADER
#define BackNMEABridge_HEADER

#include "Socket.h"
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "MOOS/libMOOSGeodesy/MOOSGeodesy.h"


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
   std::string genUVDEVString();
   void handleIncomingNMEA(std::string);
   bool ConnectToNMEAServer();
   bool failsTimeCheck(const std::string&, double& diff);
   double timeDifferenceFromNow(const std::string&);

 private: // Configuration variables
    std::string m_connect_addr = "localhost";
    unsigned short m_connect_port = 10110;
    bool validate_checksum = true;
    double maximum_time_delta = 3; // Seconds
    double attempt_reconnect_interval = 5;

 private: // State variables
    Socket m_server;
    CMOOSGeodesy m_geo;

    bool m_geo_initialized = false;

    time_t m_last_updated_time = -1;
    time_t m_last_nmea_connect_time = -1;

    double m_desired_heading = 0;
    double m_desired_speed = 0;
    double m_desired_depth = 0;
};

#endif 
