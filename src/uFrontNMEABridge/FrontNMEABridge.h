/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: FrontNMEABridge.h                                          */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#ifndef FrontNMEABridge_HEADER
#define FrontNMEABridge_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "Socket.h"
#include <iostream>

class FrontNMEABridge : public AppCastingMOOSApp
{
 public:
   FrontNMEABridge();
   ~FrontNMEABridge();

    std::string genNMEAString();
    unsigned short m_port = 10110;

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload 
   bool buildReport();

 protected:
    void registerVariables();
    void handleIncomingNMEA(std::string);

 private: // Configuration variables
    bool validate_checksum = true;
    double maximum_time_delta = 3; // Seconds

 private: // State variables
    Socket m_server;
    std::vector<std::shared_ptr<Socket>> sockets;

    time_t m_last_updated_time = -1;
    double m_latest_heading = 0;
    double m_latest_speed = 0;
    double m_latest_depth = 0;
    double m_latest_long = -1;
    double m_latest_alt = -1;
    double m_latest_lat = -1;


};

#endif 
