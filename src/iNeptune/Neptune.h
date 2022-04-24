#ifndef Neptune_HEADER
#define Neptune_HEADER

#include <GeomUtils.h>
#include <MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h>
#include <MOOS/libMOOSGeodesy/MOOSGeodesy.h>
#include "NodeRecord.h"
#include <queue>

#include "LooseNinja.h"
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
    void handleIncomingNMEA(std::string);

    std::string genMONVGString(const NodeRecord record);
    std::string genMOMISString(std::string pointSequenceId, int indexOfVisitedPoint);
    std::string genMOAVDString(std::string name, XYPolygon xyPoints);
    std::string genMODLOString(std::string obstacleID);
    static std::string genMOVALString(std::string key, std::string value, time_t time);
    std::string genMOEXTString(std::string contents);

    void handleMOPOK(std::string contents);
    void handleMOREG(std::string contents);
    void handleMOWPT(std::string contents);
    void handleMOHLM(std::string contents);
    void handleMOAVD(std::string contents);
    void handleMOGOH(std::string contents);
    void handleMOEXT(std::string contents);

    bool ConnectToNMEAServer();
    void updateWayptBehavior(std::string id);

    bool LatLonToSeglist(std::string pointsStr, XYSegList &segList);
    bool SeglistToLatLon(XYSegList seglist, std::string& newString);

    void handleNinjaEvents();

 private: // Configuration variables
    bool validate_checksum = true;
    double maximum_time_delta = 3; // Seconds
    double attempt_reconnect_interval = 5;

 private: // State variables
    LooseNinja m_ninja;

    CMOOSGeodesy m_geo;
    bool m_geo_initialized = false;

    XYSegList points;

    std::queue<std::string> send_queue;
    std::vector<std::string> forward_mail;

    time_t m_last_nmea_connect_time = -1;

    // MOMIS
    std::string m_deploy_val;
    std::string m_allstop_val;
    bool m_override_state;

    // MOWPT
    std::string m_tracking_sequence_id;
};

#endif
