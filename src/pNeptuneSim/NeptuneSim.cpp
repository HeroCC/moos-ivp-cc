/************************************************************/
/*    NAME: Conlan Cesar                                    */
/*    ORGN: MIT                                             */
/*    FILE: NeptuneSim.cpp                             */
/*    DATE: Spring 2020                                     */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "NeptuneSim.h"

using namespace std;

//---------------------------------------------------------
// Constructor

NeptuneSim::NeptuneSim()
{
}

//---------------------------------------------------------
// Destructor

NeptuneSim::~NeptuneSim()
{
}

// Converts a string 'pts={12.3,4.5:67.8,9.0}' to lat lon of same format
string NeptuneSim::PointsStrToLatLon(string pointsStr) {
  MOOSChomp(pointsStr, "{");
  pointsStr = biteStringX(pointsStr, '}');
  string newPoints = "{";
  string curPointString = biteStringX(pointsStr, ':');
  while (!curPointString.empty()) {
    double lat, lon, x, y;
    try {
      x = stod(biteStringX(curPointString, ','));
      y = stod(curPointString);
    } catch (invalid_argument &e) {
      reportRunWarning("Unable to parse latitude / longitude!");
      return "{}";
    }
    m_geo.UTM2LatLong(x, y, lat, lon);
    newPoints += doubleToStringX(lat) + "," + doubleToStringX(lon);
    curPointString = biteStringX(pointsStr, ':');
    if (!curPointString.empty()) newPoints += ":";
  }
  return newPoints + "}";
}

void NeptuneSim::handleIncomingNMEA(const string _rx) {
  string rx = _rx;
  MOOSTrimWhiteSpace(rx);

  // Verify Checksum
  string nmeaNoChecksum = rx;
  string checksum = rbiteString(nmeaNoChecksum, '*');
  string expected = "?!";
  if ((!NMEAUtils::genNMEAChecksum(nmeaNoChecksum + "*", expected) || !MOOSStrCmp(expected, checksum)) && validate_checksum) {
    reportRunWarning("Expected checksum " + expected + " but got " + checksum + ", ignoring message");
    reportEvent("Dropped Message: " + rx);
    return;
  }

  // Process Message
  string key = biteStringX(nmeaNoChecksum, ',');
  string sent_time = biteStringX(nmeaNoChecksum, ',');

  // Check Time
  double diff = -1;
  if (NMEAUtils::failsTimeCheck(sent_time, diff, maximum_time_delta)) {
    reportRunWarning("Time difference " + doubleToStringX(diff) + ">" + doubleToStringX(maximum_time_delta) + ", ignoring message " + key);
    return;
  }

  if (MOOSStrCmp(key, "$MONVG")) {
    // $MONVG,timestampOfLastMessage,lat,lon,quality(1good 0bad),altitude,depth,heading,roll,pitch,speed*
    double lat = stod(biteStringX(nmeaNoChecksum, ','));
    double lon = stod(biteStringX(nmeaNoChecksum, ','));
    bool gps_quality = false;
    setBooleanOnString(gps_quality, biteStringX(nmeaNoChecksum, ','));
    double altitude = stod(biteStringX(nmeaNoChecksum, ','));
    double depth = stod(biteStringX(nmeaNoChecksum, ','));
    double heading = stod(biteStringX(nmeaNoChecksum, ','));
    biteStringX(nmeaNoChecksum, ','); // roll
    biteStringX(nmeaNoChecksum, ','); // pitch
    double speed = stod(biteStringX(nmeaNoChecksum, ','));

    if (m_geo_initialized) {
      double x, y;
      m_geo.LatLong2LocalUTM(lat, lon, y, x);

      m_Comms.Notify("NAV_X", x);
      m_Comms.Notify("NAV_Y", y);
    }

    m_Comms.Notify("NAV_HEADING", heading);
    m_Comms.Notify("NAV_SPEED", speed);
    m_Comms.Notify("NAV_DEPTH", depth);
    m_Comms.Notify("NAV_LAT", lat);
    m_Comms.Notify("NAV_LONG", lon);
  } else if (MOOSStrCmp(key, "$MOMIS")) {
    // $MOMIS,time,{visited,point},deployed,allstop*XX
    string visited = biteStringX(nmeaNoChecksum, '}');
    MOOSChomp(nmeaNoChecksum, ",");
    string remainingPoints = biteStringX(nmeaNoChecksum, ',');
    string deployed = biteStringX(nmeaNoChecksum, ',');
    string allstop = biteStringX(nmeaNoChecksum, ',');

    string eventMsg;
    if (visited != "{") {
      eventMsg += "Visited " + visited.substr(1, visited.length() - 1) + ", ";
    }
    eventMsg += "Remaining Points: " + remainingPoints + ", ";
    eventMsg += "Deployed: " + deployed + ", ";
    eventMsg += "Allstop: " + allstop;

    reportEvent(eventMsg);
  } else {
    reportRunWarning("Unhandled Command: " + key);
  }
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool NeptuneSim::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key    = msg.GetKey();

#if 0 // Keep these around just for template
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString(); 
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif

    if (key == "APPCAST_REQ") return true;

     if(key == "REQUEST_WAYPOINTS") {
       // (true / false),{123,456}
       string val = msg.GetString();
       string reset = biteStringX(val, ',');
       send_queue.push(NMEAUtils::genNMEAString("MOWPT", reset + "," + PointsStrToLatLon(val)));
     } else if (key == "REQUEST_AVOIDANCE") {
       // pts={75,-89.4:79.6,-94:79.6,-100.6:...},label=ob1
       string val = msg.GetString();
       string points = biteStringX(val, '}');
       MOOSChomp(val, ",label=");
       send_queue.push(NMEAUtils::genNMEAString("MOAVD", val + "," + PointsStrToLatLon(points)));
     } else if (key == "NEPTUNE_DEPLOY") {
       // true or false
       bool deploy = false;
       setBooleanOnString(deploy, msg.GetString());
       send_queue.push(NMEAUtils::genNMEAString("MOHLM", boolToString(deploy) + "," + boolToString(!deploy)));
     } else {
       reportRunWarning("Unhandled Mail: " + key);
     }
  }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool NeptuneSim::OnConnectToServer()
{
  registerVariables();
  return(true);
}

bool NeptuneSim::BeginServingNMEA() {
  if (!m_server.create()) {
    reportRunWarning("Failed to create socket");
    return false;
  }
  if (!m_server.bind(m_host, m_port)) {
    reportRunWarning("Failed to bind to port " + intToString(m_port));
    return false;
  }
  if (!m_server.listen()) {
    reportRunWarning("Failed to listen to socket");
    return false;
  }
  m_server.set_non_blocking(true);
  return true;
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool NeptuneSim::Iterate()
{
  AppCastingMOOSApp::Iterate();

  // Check for new incoming connections
  std::shared_ptr<Socket> client = std::make_shared<Socket>();
  if (m_server.accept(*client)) {
    client->set_non_blocking(true); // This may be necessary for Docker (or Linux in general?)
    sockets.push_back(std::move(client));
  }

  // Remove invalid sockets
  // Good tip from https://www.fluentcpp.com/2018/09/18/how-to-remove-pointers-from-a-vector-in-cpp/
  // It's inefficient to loop over the array twice, but we'll usually only have 2 clients at most so it's not too bad
  sockets.erase(std::remove_if(sockets.begin(), sockets.end(), [](const shared_ptr<Socket>& socket){ return !socket->is_valid(); }), sockets.end());

  while (!send_queue.empty()) {
    string nmea = send_queue.front();
    send_queue.pop();
    for (const shared_ptr<Socket> &socket : sockets) {
      if (socket->is_valid()) {
        // Tx NMEA String to all attached clients
        int retval = socket->send(nmea + "\n");
        reportEvent("Sending " + nmea);
        if (retval && retval != EPIPE) {
          std::string err = strerror(retval);
          reportRunWarning("Unable to send to socket: " + err);
        } else if (retval == EPIPE) {
          // EPIPE essentially means the socket is closed, so we should mark it explicitly
          socket->close();
        }
      }
    }
  }

  for (const shared_ptr<Socket> &socket : sockets) {
    // Rx
    std::string rx;
    int len = socket->recv(rx); // TODO explicit error checking
    if (len > 0) {
      // Check if we have multiple incoming strings
      for (string &rxi : parseString(rx, "\n")) {
        // Check if we have a valid NMEA string
        if (rxi.rfind('$', 0) == 0) {
          // Process NMEA string
          Notify("INCOMING_NMEA", rxi);
          handleIncomingNMEA(rxi);
        } else {
          MOOSTrimWhiteSpace(rxi);
          reportEvent("NMEA NOTE: " + rxi);
        }
      }
    }
  }

  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool NeptuneSim::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams))
    reportConfigWarning("No config block found for " + GetAppName());

  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = tolower(biteStringX(line, '='));
    string value = line;

    bool handled = false;
    if(param == "port") {
      if (isNumber(value)) {
        int port = stoi(value);
        if (port > 65535 || port < 1) {
          reportConfigWarning("Port " + value + " is outside valid port range 1-65535");
        } else {
          m_port = stoi(value);
        }
      } else {
        reportConfigWarning("Unable to parse requested port " + value + " to int");
      }
      handled = true;
    } else if (param == "host") {
      m_host = value;
      handled = true;
    } else if (param == "validatechecksum") {
      if (!setBooleanOnString(validate_checksum, value)) {
        reportConfigWarning(param + " is not set to true or false, skipping");
      }
      handled = true;
    } else if (param == "maximumtimedifference" || param == "maximumtimedelta") {
      if (!setDoubleOnString(maximum_time_delta, value)) {
        reportConfigWarning(param + " is not a number, skipping");
      }
      handled = true;
    }

    if(!handled)
      reportUnhandledConfigWarning(orig);

  }

  double latOrigin = 0.0;
  double lonOrigin = 0.0;
  if (!(m_MissionReader.GetValue("LatOrigin", latOrigin) && m_MissionReader.GetValue("LongOrigin", lonOrigin)
        && (m_geo_initialized = m_geo.Initialise(latOrigin, lonOrigin)))) {
    reportRunWarning("Error calculating datum! XY Local Grid Unavaliable");
  }

  BeginServingNMEA();
  
  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void NeptuneSim::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();

  Register("NEPTUNE_DEPLOY", 0);
  Register("REQUEST_WAYPOINTS", 0);
  Register("REQUEST_AVOIDANCE", 0);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool NeptuneSim::buildReport() 
{
  std::string host = "UNKNOWN";
  host = inet_ntoa(m_server.get_addr().sin_addr);
  if (!MOOSStrCmp(host, m_host)) {
    host = m_host + " (" + host + ")";
  }

  m_msgs << "Listening on:     " << host << ":" << m_port << endl;
  m_msgs << "Attached Clients: " << sockets.size() << endl;
  //m_msgs << genNMEAString() << endl;

  return(true);
}