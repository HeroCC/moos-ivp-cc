/************************************************************/
/*    NAME: Conlan Cesar                                    */
/*    ORGN: MIT                                             */
/*    FILE: BackNMEABridge.cpp                              */
/*    DATE: Winter 2019-20                                  */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "BackNMEABridge.h"

using namespace std;

//---------------------------------------------------------
// Constructor

BackNMEABridge::BackNMEABridge()
{
}

//---------------------------------------------------------
// Destructor

BackNMEABridge::~BackNMEABridge()
{
  m_server.close();
}

string genNMEAChecksum(string nmeaString) {
  unsigned char xCheckSum = 0;
  string::iterator p;

  // Get the text between the $ and *
  MOOSChomp(nmeaString,"$");
  string sToCheck = MOOSChomp(nmeaString,"*");

  // XOR every byte as a checksum
  for (p = sToCheck.begin(); p != sToCheck.end(); p++) {
    xCheckSum ^= *p;
  }

  ostringstream os;
  os.flags(ios::hex);
  os << (int) xCheckSum;
  string sExpected = os.str();

  if (sExpected.length() < 2)
    sExpected = "0" + sExpected;

  return sExpected;
}

string BackNMEABridge::genUVDEVString() {
  const int precision = 7;
  string nmea = "$UVDEV,";
  std::stringstream ss;
  ss << std::put_time(std::localtime(&m_last_updated_time), "%H%M%S,");
  nmea += ss.str();
  nmea += doubleToString(m_desired_heading, precision) + "," + doubleToString(m_desired_speed, precision) + "," + doubleToString(m_desired_depth, precision) + "*";
  nmea += genNMEAChecksum(nmea);
  return nmea;
}

double BackNMEABridge::timeDifferenceFromNow(const string& t2) {
  const time_t currtime = m_curr_time;

  struct std::tm* tm = localtime(&currtime); // Assume we have the same timezone
  std::istringstream ss(t2);
  ss >> std::get_time(tm, "%H%M%S"); // Override hour, minute, second
  //strptime(sent_time.c_str(), "%H%M%S", &tm);
  time_t tx_unix_time = mktime(tm);

  return abs(currtime - tx_unix_time);
}

bool BackNMEABridge::failsTimeCheck(const string& t2, double& diff) {
  if (maximum_time_delta < 0) {
    return false; // If time delta is below zero, consider it disabled
  }
  double df = timeDifferenceFromNow(t2);
  diff = df;
  return (df > maximum_time_delta);
}

void BackNMEABridge::handleIncomingNMEA(const string _rx) {
  string rx = _rx;
  MOOSTrimWhiteSpace(rx);

  // Verify Checksum
  string nmeaNoChecksum = rx;
  string checksum = rbiteString(nmeaNoChecksum, '*');
  string expected = genNMEAChecksum(nmeaNoChecksum);
  if (!MOOSStrCmp(expected, checksum) && validate_checksum) {
    reportRunWarning("Expected checksum " + expected + " but got " + checksum + ", ignoring message");
    reportEvent("Dropped Message: " + rx);
    return;
  }
  // All messages follow $KEYHI,TimestampSent,the rest*XX

  // Process Message
  string key = biteStringX(nmeaNoChecksum, ',');
  string sent_time = biteStringX(nmeaNoChecksum, ',');

  // Check Time
  double diff = -1;
  if (failsTimeCheck(sent_time, diff)) {
    reportRunWarning("Time difference " + doubleToString(diff) + ">" + doubleToString(maximum_time_delta) + ", ignoring message " + key);
    return;
  }

  if (MOOSStrCmp(key, "$MONVG")) {
    // $MONVG,timestampOfLastMessage,lat,,lon,,quality(1good 0bad),altitude,depth,heading,speed*
    double lat = stod(biteStringX(nmeaNoChecksum, ','));
    biteStringX(nmeaNoChecksum, ','); // blank
    double lon = stod(biteStringX(nmeaNoChecksum, ','));
    biteStringX(nmeaNoChecksum, ','); // blank
    bool gps_quality = false;
    setBooleanOnString(gps_quality, biteStringX(nmeaNoChecksum, ','));
    double altitude = stod(biteStringX(nmeaNoChecksum, ','));
    double depth = stod(biteStringX(nmeaNoChecksum, ','));
    double heading = stod(biteStringX(nmeaNoChecksum, ','));
    double speed = stod(biteStringX(nmeaNoChecksum, ','));

    if (m_geo_initialized) {
      double x;
      double y;
      m_geo.LatLong2LocalUTM(lat, lon, y, x);

      m_Comms.Notify("NAV_X", x);
      m_Comms.Notify("NAV_Y", y);
    }

    m_Comms.Notify("NAV_HEADING", heading);
    m_Comms.Notify("NAV_SPEED", speed);
    m_Comms.Notify("NAV_DEPTH", depth);
    m_Comms.Notify("NAV_LAT", lat);
    m_Comms.Notify("NAV_LONG", lon);
  } else if (MOOSStrCmp(key, "$MOPOK")) {
    // $MONVG,timestampOfLastMessage,KEY=val*
    // Poke a MoosDB message
    string mail = biteStringX(nmeaNoChecksum, '=');
    string val = nmeaNoChecksum; // the only remaining text should be the value to set
    if (isNumber(val)) {
      Notify(mail, stod(val));
    } else {
      Notify(mail, val);
    }
  } else {
    reportRunWarning("Unhandled Command: " + key);
  }
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool BackNMEABridge::OnNewMail(MOOSMSG_LIST &NewMail)
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

    if(key == "DESIRED_HEADING") {
      m_desired_heading = msg.GetDouble();
    } else if (key == "DESIRED_SPEED") {
      m_desired_speed = msg.GetDouble();
    } else if (key == "DESIRED_DEPTH") {
      m_desired_depth = msg.GetDouble();
    } else if (key == "IVPHELM_STATE") {
      // Heartbeat, means helm is alive so it's safe to set last updated time
    } else if(key != "APPCAST_REQ") { // handled by AppCastingMOOSApp
      reportRunWarning("Unhandled Mail: " + key);
      return true;
    }
    m_last_updated_time = m_curr_time;

   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool BackNMEABridge::OnConnectToServer()
{
  double latOrigin = 0.0;
  double lonOrigin = 0.0;
  if (!(m_MissionReader.GetValue("LatOrigin", latOrigin) && m_MissionReader.GetValue("LongOrigin", lonOrigin)
        && (m_geo_initialized = m_geo.Initialise(latOrigin, lonOrigin)))) {
    reportRunWarning("Error calculating datum! XY Local Grid Unavaliable");
  }

  registerVariables();
  return(true);
}

//---------------------------------------------------------
// Procedure: ConnectToNMEAServer

bool BackNMEABridge::ConnectToNMEAServer()
{
  m_last_nmea_connect_time = m_curr_time;
  m_server.close();

  std::string niceAddr = m_connect_addr + ":" + intToString(m_connect_port);
  reportEvent("Attempting to connect to server @ " + niceAddr);

  if (!m_server.create()) {
    reportRunWarning("Failed to create socket");
    m_server.close();
    return false;
  }

  int retval = m_server.connect(m_connect_addr, m_connect_port);
  if (retval) {
    reportRunWarning("Failed to connect to server");
    std::string err = strerror(retval);
    reportEvent("Failure Reason: " + err);
    m_server.close();
    return false;
  }
  m_server.set_non_blocking(true);

  return true;
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool BackNMEABridge::Iterate()
{
  AppCastingMOOSApp::Iterate();

  string nmea = genUVDEVString();
  Notify("GENERATED_NMEA_UVDEV", nmea);

  if (m_server.is_valid()) {
    // Tx NMEA String to server
    int retval = m_server.send(nmea + "\n");
    if (retval) {
      std::string err = strerror(retval);
      reportRunWarning("Lost connection to server: " + err);
      m_server.close();
    }

    // Rx
    std::string rx;
    int len = m_server.recv(rx); // TODO explicit error checking
    if (len > 0) {
      // Check if we have multiple incoming strings
      for (string& rxi : parseString(rx, "\n")) {
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

    retractRunWarning("Failed to connect to server"); // If we're connected, we no longer need the warning
  } else {
    if (m_curr_time - m_last_nmea_connect_time >= attempt_reconnect_interval) {
      ConnectToNMEAServer();
    }
  }

  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool BackNMEABridge::OnStartUp()
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
          m_connect_port = stoi(value);
        }
      } else {
        reportConfigWarning("Unable to parse requested port " + value + " to int");
      }
      handled = true;
    } else if (param == "host") {
      m_connect_addr = value;
      handled = true;
    } else if (param == "reconnectinterval") {
      attempt_reconnect_interval = stod(value);
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
  
  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void BackNMEABridge::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("DESIRED_HEADING", 0);
  Register("DESIRED_DEPTH", 0);
  Register("DESIRED_SPEED", 0);
  Register("IVPHELM_STATE", 0); // Heartbeat
}


//------------------------------------------------------------
// Procedure: buildReport()

bool BackNMEABridge::buildReport() 
{
  m_msgs << "Valid Connection: " << boolToString(m_server.is_valid()) << endl;
  m_msgs << "Host: " << m_connect_addr << endl;
  m_msgs << "Port: " << intToString(m_connect_port) << endl;

  return(true);
}




