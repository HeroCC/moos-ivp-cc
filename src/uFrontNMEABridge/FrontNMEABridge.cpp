/************************************************************/
/*    NAME: Conlan Cesar                                    */
/*    ORGN: MIT                                             */
/*    FILE: FrontNMEABridge.cpp                             */
/*    DATE: Winter 2019                                     */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "FrontNMEABridge.h"

using namespace std;

//---------------------------------------------------------
// Constructor

FrontNMEABridge::FrontNMEABridge()
{
}

//---------------------------------------------------------
// Destructor

FrontNMEABridge::~FrontNMEABridge()
{
}

// Algorithm from Alon's iM200 code, thank you!
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

string FrontNMEABridge::genMONVGString() {
  // Very similar to CPNVG from https://oceanai.mit.edu/herons/docs/ClearpathWireProtocolV0.2.pdf
  // $MONVG,timestampOfLastMessage,lat,,lon,,quality(1good 0bad),altitude,depth,heading,speed_over_ground*
  string nmea = "$MONVG,";
  std::stringstream ss;
  ss << std::put_time(std::localtime(&m_last_updated_time), "%H%M%S.00,");
  nmea += ss.str();
  nmea += doubleToString(m_latest_lat) + ",," + doubleToString(m_latest_long) + ",,1," + doubleToString(m_latest_alt) +
    "," + doubleToString(m_latest_depth) + "," + doubleToString(m_latest_heading) +  "," + doubleToString(m_latest_speed) + "*";
  nmea += genNMEAChecksum(nmea);
  return nmea;
}

void FrontNMEABridge::handleIncomingNMEA(const string _rx) {
  string rx = _rx;
  MOOSTrimWhiteSpace(rx);

  // Verify Checksum
  string nmeaNoChecksum = rx;
  string checksum = rbiteString(nmeaNoChecksum, '*');
  string expected = genNMEAChecksum(nmeaNoChecksum);
  if (!MOOSStrCmp(expected, checksum) && validate_checksum) {
    reportRunWarning("Expected checksum " + expected + " but got " + checksum + ", ignoring message");
    return;
  }

  // Process Message
  string key = biteStringX(nmeaNoChecksum, ',');
  if (MOOSStrCmp(key, "$UVDEV")) {
    // $UVDEV,hhmmss.ss,heading,speed,depth*XX
    // Unmanned Vehicle DEsired Velocity(?)
    string sent_time = biteStringX(nmeaNoChecksum, ',');
    double heading = stod(biteStringX(nmeaNoChecksum, ','));
    double speed = stod(biteStringX(nmeaNoChecksum, ','));
    double depth = stod(biteStringX(nmeaNoChecksum, ','));

    // Check Time
    if (maximum_time_delta >= 0) {
      const time_t currtime = std::time(nullptr);

      struct std::tm* tm = localtime(&currtime); // Assume we have the same date
      std::istringstream ss(sent_time);
      ss >> std::get_time(tm, "%H%M%S"); // Override hour, minute, second
      //strptime(sent_time.c_str(), "%H%M%S", &tm);
      time_t tx_unix_time = mktime(tm);

      long diff = abs(currtime - tx_unix_time);
      if (diff > maximum_time_delta) {
        reportRunWarning("Time difference " + doubleToString(diff) + ">" + doubleToString(maximum_time_delta) + ", ignoring message " + key);
        return;
      }
    }

    m_Comms.Notify("DESIRED_HEADING", heading);
    m_Comms.Notify("DESIRED_SPEED", speed);
    m_Comms.Notify("DESIRED_DEPTH", depth);
  } else {
    reportRunWarning("Unhandled Command: " + key);
  }
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool FrontNMEABridge::OnNewMail(MOOSMSG_LIST &NewMail)
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

     if(key == "NAV_HEADING_OVER_GROUND" || key == "NAV_HEADING") {
       m_latest_heading = msg.GetDouble();
     } else if (key == "NAV_SPEED_OVER_GROUND" || key == "NAV_SPEED") {
       m_latest_speed = msg.GetDouble();
     } else if (key == "NAV_DEPTH") {
       m_latest_depth = msg.GetDouble();
     }  else if (key == "NAV_LAT") {
       m_latest_lat = msg.GetDouble();
     } else if (key == "NAV_LONG") {
       m_latest_long = msg.GetDouble();
     } else if (key == "NAV_ALTITUDE") {
       m_latest_alt = msg.GetDouble();
     } else if(key != "APPCAST_REQ") { // handled by AppCastingMOOSApp
       reportRunWarning("Unhandled Mail: " + key);
     }
     m_last_updated_time = std::time(nullptr);

   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool FrontNMEABridge::OnConnectToServer()
{
  if (!m_server.create()) {
    reportRunWarning("Failed to create socket");
    return false;
  }
  if (!m_server.bind(m_port)) {
    reportRunWarning("Failed to bind to port " + intToString(m_port));
    return false;
  }
  if (!m_server.listen()) {
    reportRunWarning("Failed to listen to socket");
    return false;
  }
  m_server.set_non_blocking(true);

  registerVariables();
  return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool FrontNMEABridge::Iterate()
{
  AppCastingMOOSApp::Iterate();
  string nmea = genMONVGString();

  Notify("GENERATED_NMEA_MONVG", nmea);

  // Check for new incoming connections
  std::shared_ptr<Socket> client = std::make_shared<Socket>(); // TODO for safety, make this a unique_ptr
  if (m_server.accept(*client)) {
    client->send("JOINED\n");
    sockets.push_back(std::move(client));
  }

  // Remove invalid sockets
  // Good tip from https://www.fluentcpp.com/2018/09/18/how-to-remove-pointers-from-a-vector-in-cpp/
  // It's inefficient to loop over the array twice, but we'll usually only have 2 clients at most so it's not too bad
  sockets.erase(std::remove_if(sockets.begin(), sockets.end(), [](const shared_ptr<Socket>& socket){ return !socket->is_valid(); }), sockets.end());

  for (const shared_ptr<Socket>& socket : sockets) {
    if (socket->is_valid()) {
      // Tx NMEA String to all attached clients
      int retval = socket->send(nmea + "\n");
      if (retval && retval != EPIPE) {
        std::string err = strerror(retval);
        reportRunWarning("Unable to send to socket: " + err);
      } else if (retval == EPIPE) {
        // EPIPE essentially means the socket is closed, so we should mark it explicitly
        socket->close();
      }

      // Rx
      std::string rx;
      int len = socket->recv(rx); // TODO explicit error checking
      if (len > 0) {
        // Check if we have a valid NMEA string
        if (rx.rfind('$', 0) == 0) {
          // Process NMEA string
          Notify("INCOMING_NMEA", rx);
          handleIncomingNMEA(rx);
        } else {
          MOOSTrimWhiteSpace(rx);
          reportEvent(rx);
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

bool FrontNMEABridge::OnStartUp()
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

void FrontNMEABridge::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();

  Register("NAV_SPEED", 0);
  Register("NAV_HEADING", 0);
  //Register("NAV_HEADING_OVER_GROUND", 0);
  //Register("NAV_SPEED_OVER_GROUND", 0);
  Register("NAV_DEPTH", 0);
  Register("NAV_LAT", 0);
  Register("NAV_LONG", 0);
  Register("NAV_ALTITUDE", 0);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool FrontNMEABridge::buildReport() 
{
  m_msgs << "Listening on:     " << inet_ntoa(m_server.get_addr().sin_addr) << ":" << m_port << endl;
  m_msgs << "Attached Clients: " << sockets.size() << endl;
  //m_msgs << genNMEAString() << endl;

  return(true);
}