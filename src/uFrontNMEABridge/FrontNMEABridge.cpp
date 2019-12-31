/************************************************************/
/*    NAME: Conlan Cesar                                    */
/*    ORGN: MIT                                             */
/*    FILE: FrontNMEABridge.cpp                             */
/*    DATE: Winter 2019                                     */
/*    Bits and pieces taken from Alon's iM200 Code          */
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

string FrontNMEABridge::genNMEAString() {
  // Very similar to CPNVG from https://oceanai.mit.edu/herons/docs/ClearpathWireProtocolV0.2.pdf
  // $MONVG,timestampOfLastMessage,lat,,lon,,quality(1good 0bad),altitude,depth,heading,speed_over_ground*
  string nmea = "$MORMV,";
  std::stringstream ss;
  ss << std::put_time(std::localtime(&m_last_updated_time), "%H%M%S.00,");
  nmea += ss.str();
  nmea += doubleToString(m_latest_lat) + ",," + doubleToString(m_latest_long) + ",,1," + doubleToString(m_latest_alt) +
    "," + doubleToString(m_latest_depth) + "," + doubleToString(m_latest_heading) +  "," + doubleToString(m_latest_speed) + "*";
  nmea += genNMEAChecksum(nmea);
  return nmea;
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

     if(key == "NAV_HEADING_OVER_GROUND") {
       m_latest_heading = msg.GetDouble();
     } else if (key == "NAV_SPEED_OVER_GROUND") {
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
     m_last_updated_time = m_curr_time;

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
  string nmea = genNMEAString();

  Notify("GENERATED_NMEA_STRING", nmea);

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
      if (retval) {
        std::string err = strerror(retval);
        reportRunWarning("Unable to send to socket: " + err);
      }

      // Rx
      std::string rx;
      int len = socket->recv(rx); // TODO explicit error checking
      if (len > 0) {
        reportEvent(rx);
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
    if(param == "foo") {
      handled = true;
    }
    else if(param == "bar") {
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

  Register("NAV_HEADING_OVER_GROUND", 0);
  Register("NAV_SPEED_OVER_GROUND", 0);
  Register("NAV_DEPTH", 0);
  Register("NAV_LAT", 0);
  Register("NAV_LONG", 0);
  Register("NAV_ALTITUDE", 0);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool FrontNMEABridge::buildReport() 
{
  m_msgs << "Attached Clients: " << sockets.size() << endl;
  m_msgs << genNMEAString() << endl;

  return(true);
}