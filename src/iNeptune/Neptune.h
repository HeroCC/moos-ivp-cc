/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: Neptune.h                                          */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#ifndef Neptune_HEADER
#define Neptune_HEADER

#include <iostream>

#include <GeomUtils.h>
#include <MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h>
#include <MOOS/libMOOSGeodesy/MOOSGeodesy.h>


#include "Socket.h"

class Neptune : public AppCastingMOOSApp
{
 public:
   Neptune();
   ~Neptune();

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload 
   bool buildReport();

 protected:
    std::string genMONVGString();
    void registerVariables();
    void handleIncomingNMEA(std::string);
    bool ConnectToNMEAServer();
    bool failsTimeCheck(const std::string&, double& diff);
    double timeDifferenceFromNow(const std::string&);
    static std::string genNMEAChecksum(std::string);
    static std::string genMOVALString(std::string key, std::string value, time_t time);

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

    XYSegList points;

    std::vector<std::string> send_queue;
    std::vector<std::string> forward_mail;

    time_t m_last_nmea_connect_time = -1;

    // MONVG
    time_t m_last_updated_time = -1;
    double m_latest_heading = 0;
    double m_latest_speed = 0;
    double m_latest_depth = 0;
    double m_latest_long = -1;
    double m_latest_alt = -1;
    double m_latest_lat = -1;
};

#endif 
