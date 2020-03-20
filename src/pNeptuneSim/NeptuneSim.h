/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: NeptuneSim.h                                          */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#ifndef NeptuneSim_HEADER
#define NeptuneSim_HEADER

#include <GeomUtils.h>
#include <MOOS/libMOOSGeodesy/MOOSGeodesy.h>
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "NMEAUtils.h"
#include "Socket.h"
#include <iostream>
#include <queue>

class NeptuneSim : public AppCastingMOOSApp
{
 public:
   NeptuneSim();
   ~NeptuneSim();

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload
   void registerVariables();
   bool buildReport();

 protected:
    std::string PointsStrToLatLon(std::string pointsStr);
    void handleIncomingNMEA(std::string);
    bool BeginServingNMEA();

 private: // Configuration variables
    std::string m_host = "0.0.0.0";
    unsigned short m_port = 10110;
    bool validate_checksum = true;
    double maximum_time_delta = 3; // Seconds

 private: // State variables
    Socket m_server;
    std::vector<std::shared_ptr<Socket>> sockets;

    std::queue<std::string> send_queue;

    CMOOSGeodesy m_geo;
    bool m_geo_initialized = false;

};

#endif 
