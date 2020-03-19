/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: Neptune.h                                          */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#ifndef Neptune_HEADER
#define Neptune_HEADER

#include <GeomUtils.h>
#include <MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h>
#include <MOOS/libMOOSGeodesy/MOOSGeodesy.h>
#include <queue>

#include "Socket.h"
#include "NMEAUtils.h"

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
   void registerVariables();

 protected:
    std::string genMONVGString();
    std::string genMOMISString(double *x, double *y);
    static std::string genMOVALString(std::string key, std::string value, time_t time);

    void handleIncomingNMEA(std::string);
    bool ConnectToNMEAServer();
    void UpdateBehaviors();

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

    std::queue<std::string> send_queue;
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

    // MOMIS
    std::string m_deploy_val;
    std::string m_allstop_val;
};

#endif 
