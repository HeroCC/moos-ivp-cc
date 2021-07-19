/************************************************************/
/*    NAME: Conlan Cesar                                    */
/*    ORGN: MIT                                             */
/*    FILE: Neptune.cpp                             */
/*    DATE: Spring 2020                                     */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "Neptune.h"
#include "XYFormatUtilsPoly.h"
#include "NodeRecordUtils.h"

using namespace std;

//---------------------------------------------------------
// Constructor

Neptune::Neptune()
{
  m_ninja = LooseNinja("client", 10110);
  m_ninja.m_validate_checksum = false;
}

//---------------------------------------------------------
// Destructor

Neptune::~Neptune()
{
}

// ----------------------
// Generate NMEA Messages 

string Neptune::genMONVGString(const NodeRecord record) {
  // Very similar to CPNVG from https://oceanai.mit.edu/herons/docs/ClearpathWireProtocolV0.2.pdf
  // $MONVG,timestampOfLastMessage,lat,lon,quality(1good 0bad),altitude,depth,heading,roll,pitch,speed*
  string contents;
  contents += doubleToString(record.getLat(), 6) + "," + doubleToString(record.getLon(), 6) + ","; // Lat & lon
  contents += intToString(m_geo_initialized && record.isSetLongitude()) + "," + doubleToString(record.getAltitude(), 2) + ","; // Altitude & status
  contents += doubleToString(record.getDepth(), 2) + "," + doubleToString(record.getHeading(), 3) + ",," + doubleToString(record.getPitch()) + "," + doubleToString(record.getSpeed(), 2);
  return NMEAUtils::genNMEAString("MONVG", contents, record.getTimeStamp());
}

string Neptune::genMOVALString(std::string key, std::string value, time_t time) {
  return NMEAUtils::genNMEAString("MOVAL", key + "," + value, time);
}

string Neptune::genMOMISString(std::string sequenceName, int visitedIndex) {
  // $MOMIS,timestamp,sequenceName,ackWaypointIndex,deployed,allstop_reason*XX
  string value = sequenceName + "," + (visitedIndex >= 0 ? intToString(visitedIndex) : "") + "," + m_deploy_val + "," + m_allstop_val;
  return NMEAUtils::genNMEAString("MOMIS", value);
}

string Neptune::genMOAVDString(string name, XYPolygon xyPoints) {
  string latLonString = "";
  if (!SeglistToLatLon(xyPoints.exportSegList(), latLonString)) {
    reportRunWarning("Not sending $MOAVD message, we couldn't parse the points!");
    return NMEAUtils::genNMEAString("MOAVD", name + ",ERROR");
  }
  return NMEAUtils::genNMEAString("MOAVD", name + "," + latLonString);
}

string Neptune::genMODLOString(string obstacleID) {
  return NMEAUtils::genNMEAString("MODLO", obstacleID);
}

// TODO use proper seglist -- needs mike to tweak pop_last_vertex to return XYPoint
bool Neptune::SeglistToLatLon(XYSegList seglist, string& newString) {
  string specPts = seglist.get_spec_pts();
  MOOSChomp(specPts, "{"); // Remove the surrounding "pts={...}"
  specPts = MOOSChomp(specPts, "}");

  string latLon = "{";
  string curPointString;
  while (!(curPointString = biteStringX(specPts, ':')).empty()) {
    double lat, lon, x, y;
    try {
      x = stod(biteStringX(curPointString, ','));
      y = stod(curPointString);
    } catch (invalid_argument& e) {
      reportRunWarning("Unable to parse x / y coord!");
      return false;
    }
    if (!m_geo.UTM2LatLong(x, y, lat, lon)) {
      reportRunWarning("Unable to translate XY to Lat/Lon! Is geodessy initialized?");
      return false;
    }
    latLon += doubleToStringX(lat, 6) + "," + doubleToStringX(lon) + (specPts.empty() ? "}" : ":");
  }
  newString = latLon;
  return true;
}

// --------------------
// Handle Incoming NMEA

bool Neptune::LatLonToSeglist(string pointsStr, XYSegList& segList) {
  string curPointString = biteStringX(pointsStr, ':');
  while (!curPointString.empty()) {
    double lat, lon, x, y;
    try {
      lat = stod(biteStringX(curPointString, ','));
      lon = stod(curPointString);
    } catch (invalid_argument& e) {
      reportRunWarning("Unable to parse latitude / longitude!");
      return false;
    }
    m_geo.LatLong2LocalUTM(lat, lon, y, x);
    segList.add_vertex(x, y);
    curPointString = biteStringX(pointsStr, ':');
  }
  return true;
}

void Neptune::updateWayptBehavior(std::string sequenceId) {
  m_tracking_sequence_id = sequenceId;
  if (points.size() > 0) {
    string pointsStr = "points=" + points.get_spec();
    string wptFlagWithId = "wptflag = NEPTUNE_SURVEY_VISITED_POINT = id=" + sequenceId + ",utc=$[UTC],px=$[PX],py=$[PY],pi=$[PI]";
    string wptNextFlagWithId = "wptflag = NEPTUNE_SURVEY_NEXT_POINT = id=" + sequenceId + ",utc=$[UTC],nx=$[NX],ny=$[NY],ni=$[NI]";
    Notify("NEPTUNE_SURVEY_UPDATE", pointsStr + " # " + wptFlagWithId + " # " + wptNextFlagWithId);
    Notify("NEPTUNE_SURVEY_TRAVERSE", "true");
  } else {
    // Sending an empty list of points makes the behavior get upset, so use a condition flag
    Notify("NEPTUNE_SURVEY_TRAVERSE", "false");
  }
}

void Neptune::handleMOPOK(string contents) {
  // $MOPOK,timestampOfLastMessage,KEY=val*
  // Poke a MoosDB message
  string mail = biteStringX(contents, '=');
  string val = contents; // the only remaining text should be the value to set
  if (isNumber(val)) {
    Notify(mail, stod(val));
  } else {
    Notify(mail, val);
  }
}

void Neptune::handleMOREG(string contents) {
  // Register a moos variable
  // $MOREG,timestamp,Key To Register,dfInterval*
  string regKey = biteStringX(contents, ',');
  double dfInterval = stod(biteStringX(contents, ','));

  forward_mail.push_back(regKey);
  Register(regKey, dfInterval);
}

void Neptune::handleMOWPT(string contents) {
  // $MOWPT,timestamp,waypointSequenceId,reset,{lat1,lon1:lat2,lon2:...:latN,lonN}*XX
  // Set or add to XYSegList of Points. Relies on a valid m_geo.
  // Since we translate from lat,lon -> XY, we may lose precision as we get extremely far from datum.
  if (!m_geo_initialized) {
    reportRunWarning("Waypoints requested, but Geo isn't ready! Did you set a datum?");
    return;
  }

  bool reset;
  string sequenceId = biteStringX(contents, ',');
  string resetStr = biteStringX(contents, ',');
  setBooleanOnString(reset, resetStr, true);

  int oldSize = points.size();

  if (reset) {
    points.clear();
  }

  MOOSChomp(contents, "{");
  string pointsString = MOOSChomp(contents, "}");

  if(LatLonToSeglist(pointsString, points)) {
    updateWayptBehavior(sequenceId);
    reportEvent("Updated waypoint sequence [" + sequenceId + "], remaining is now " + intToString(points.size()) + " (was " + intToString(oldSize) + ")");
  }
}

void Neptune::handleMOHLM(string contents) {
  // $MOHLM,timestamp,deploy,manual_override*XX
  // Access key helm values. Deploy refers to NEPTUNE behaviors, and manual_override is the system-wide override
  string deployString = biteStringX(contents, ',');
  string manualOverrideString = biteStringX(contents, ',');

  // If either of the parsers fail, we default to disabling
  bool deploy = false;
  bool manualOverride = true;
  setBooleanOnString(deploy, deployString);
  setBooleanOnString(manualOverride, manualOverrideString);

  Notify("DEPLOY", boolToString(deploy));
  Notify("MOOS_MANUAL_OVERRIDE", boolToString(manualOverride));
  reportEvent("Updated helm state, is now: DEPLOY=" + boolToString(deploy) + ", OVERRIDE=" + boolToString(manualOverride));
}

void Neptune::handleMOGOH(string contents) {
  // $MOGOH,timestamp,heading,speed*XX
  string headingString = biteStringX(contents, ',');
  string speedString = biteStringX(contents, ',');

  bool deployState = false;
  setBooleanOnString(deployState, m_deploy_val);

  if (m_override_state) {
    // Handle MOOS_MANUAL_OVERRIDE -- do nothing
    reportEvent("Got $MOGOH request, but MOOS_MANUAL_OVERRIDE is true! Doing nothing.");
    return;
  } else if (deployState) {
    // Handle DEPLOY=true -- generally you want this to be false when using $MOGOH, but some edge cases may find it useful
    reportEvent("Got $MOGOH request, but DEPLOY is true. Continuing, but you should ensure waypoints are cleared");
  }

  double hdgVal, speedVal;
  if (!(setDoubleOnString(hdgVal, headingString) && setDoubleOnString(speedVal, speedString))) {
    // Parsing error
    reportRunWarning("Unable to parse heading / speed from $MOGOH! Doing nothing.");
    return;
  }

  Notify("DESIRED_HEADING", hdgVal);
  Notify("DESIRED_SPEED", speedVal);
  reportEvent("Neptune set new desired heading: " + doubleToStringX(hdgVal) +", speed: " + doubleToStringX(speedVal));
}

void Neptune::handleMOAVD(string contents) {
  if (!m_geo_initialized) {
    reportRunWarning("Avoidance requested, but Geo isn't ready! Did you set a datum?");
    return;
  }

  string regionID = biteStringX(contents, ',');
  MOOSChomp(contents, "{");
  string region =  MOOSChomp(contents, "}");

  XYPolygon poly;
  poly.set_label(regionID);
  LatLonToSeglist(region, poly);
  //poly.determine_convexity(); // TODO crashes on XYPolygon.cpp line 955
  if (!poly.is_convex()) {
    // pObstacleManager will warn too, but we make it easier to debug causing region
    //reportRunWarning("Requested avd region '" + regionID + "' is non-convex, can't accept");
  }
  Notify("GIVEN_OBSTACLE", poly.get_spec()); // Uses pObstacleMgr
  reportEvent("Updated Neptune ignore area: " + regionID);
}

void Neptune::handleIncomingNMEA(const string _rx) {
  string rx = _rx;
  MOOSTrimWhiteSpace(rx);

  // Verify Checksum
  string nmeaNoChecksum = rx;
  string checksum = rbiteString(nmeaNoChecksum, '*');
  string expected = "?!";
  if (validate_checksum && (!NMEAUtils::genNMEAChecksum(nmeaNoChecksum + "*", expected) || !MOOSStrCmp(expected, checksum))) {
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
  if (NMEAUtils::failsTimeCheck(sent_time, diff, maximum_time_delta)) {
    reportRunWarning("Time difference " + doubleToStringX(diff) + ">" + doubleToStringX(maximum_time_delta) + ", ignoring message " + key);
    return;
  }

  // Process Message
  if (MOOSStrCmp(key, "$MOPOK")) {
    handleMOPOK(nmeaNoChecksum);
  } else if (MOOSStrCmp(key, "$MOREG")) {
    handleMOREG(nmeaNoChecksum);
  } else if (MOOSStrCmp(key, "$MOWPT")) {
    handleMOWPT(nmeaNoChecksum);
  } else if (MOOSStrCmp(key, "$MOHLM")) {
    handleMOHLM(nmeaNoChecksum);
  } else if (MOOSStrCmp(key, "$MOAVD")) {
    handleMOAVD(nmeaNoChecksum);
  } else if (MOOSStrCmp(key, "$MOGOH")) {
    handleMOGOH(nmeaNoChecksum);
  } else {
    reportRunWarning("Unhandled Command: " + key);
  }
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool Neptune::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  // We don't want to send multiple MOMIS in a single iterations, they may go out in the wrong order
  bool sendMOMIS = false;

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

    if (vectorContains(forward_mail, key, false)) {
      send_queue.push(genMOVALString(key, msg.GetAsString(), msg.GetTime()));
    }

    if (key == "APPCAST_REQ") continue;

     if(key == "INCOMING_NMEA_MESSAGE") {
       handleIncomingNMEA(msg.GetAsString());
     } else if (key == "NODE_REPORT_LOCAL") {
       if (m_ninja.isConnected()) {
         send_queue.push(genMONVGString(string2NodeRecord(msg.GetString(), true)));
       }
     } else if (key == "OBSTACLE_INFORM") {
       string message = msg.GetString();
       string name = tokStringParse(message, "name", '#', '=');
       MOOSChomp(message, "#poly=");
       string polyStr = message;
       XYPolygon obsPoly = string2Poly(polyStr);
       if (!obsPoly.valid()) {
         reportRunWarning("Observed an obstacle alert, but unable to parse poly!");
       }
       send_queue.push(genMOAVDString(name, obsPoly));
       reportEvent("Reporting updated obstacle: " + name + ", points: " + obsPoly.get_spec_pts());
     } else if (key == "OBM_RESOLVED") {
       string name = msg.GetAsString();
       send_queue.push(genMODLOString(msg.GetAsString()));
       reportEvent("Reporting resolved obstacle: " + name);
     } else if (key == "NEPTUNE_SURVEY_VISITED_POINT") {
       // Visited a point, so remove it from list
       string val = msg.GetString();
       string sequenceId = tokStringParse(val, "id", ',', '=');
       double x, y;
       double i; // This is really an int, but tokParse doesn't like that
       if (m_tracking_sequence_id != sequenceId) {
         // The BHV_Waypoint is informing us of old previous sequence IDs -- we can't clear wptflags though, so just ignore
         // TODO ask Mike to make `wptflag = clear` reset the list of wptflags (could be expanded to other flags too)
       } else if (tokParse(val, "px", ',', '=', x) 
         && tokParse(val, "py", ',', '=', y) 
         && tokParse(val, "pi", ',', '=', i)
         ) {
         reportEvent("Visited point [" + intToString(i) + "] (" + doubleToString(x, 1) + ", " + doubleToString(y, 1) + ") from seq " + sequenceId);
         send_queue.push(genMOMISString(sequenceId, i));
         points.delete_vertex(x, y);
       } else {
         reportRunWarning("Received a visited point, but was unable to parse it: " + msg.GetString());
       }
     } else if (key == "DEPLOY") {
       m_deploy_val = msg.GetString();
       sendMOMIS = true;
     } else if (key == "IVPHELM_ALLSTOP") {
       m_allstop_val = msg.GetString();
       sendMOMIS = true;
     } else if (key == "MOOS_MANUAL_OVERRIDE") {
       setBooleanOnString(m_override_state, msg.GetString());
       sendMOMIS = true;
     } else {
       reportRunWarning("Unhandled unrequested mail: " + key);
       return true;
     }
   }

  if (sendMOMIS) {
    send_queue.push(genMOMISString(m_tracking_sequence_id, -1));
  }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool Neptune::OnConnectToServer()
{
  registerVariables();
  return(true);
}

//---------------------------------------------------------
// Procedure: ConnectToNMEAServer

bool Neptune::ConnectToNMEAServer()
{
  m_last_nmea_connect_time = m_curr_time;
  m_ninja.closeSockFDs();

  if (!m_ninja.setupConnection()) {
    m_ninja.closeSockFDs();
    return false;
  }

  return true;
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool Neptune::Iterate()
{
  AppCastingMOOSApp::Iterate();

  if (m_ninja.isConnected()) {
    // Tx NMEA String to server
    while (!send_queue.empty()) {
      std::string val = send_queue.front();
      send_queue.pop();
      Notify("SENT_NMEA_MESSAGE", val);
      m_ninja.sendSockMessage(val + "\n");
    }

    // Rx
    for (const string& rx : m_ninja.getSockMessages()) {
      Notify("INCOMING_NMEA_MESSAGE", rx);
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

bool Neptune::OnStartUp()
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
      m_ninja.setPortNumber(stoi(value));
      handled = true;
    } else if (param == "host") {
      m_ninja.setIPAddr(value);
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

  double latOrigin = 0.0;
  double lonOrigin = 0.0;
  if (!(m_MissionReader.GetValue("LatOrigin", latOrigin) && m_MissionReader.GetValue("LongOrigin", lonOrigin)
        && (m_geo_initialized = m_geo.Initialise(latOrigin, lonOrigin)))) {
    reportRunWarning("Error calculating datum! XY Local Grid Unavailable");
  }

  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void Neptune::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();

  Register("INCOMING_NMEA_MESSAGE", 0);

  // Navigation Information
  Register("NODE_REPORT_LOCAL", 0);

  // Obstacle Manager
  Register("OBSTACLE_INFORM", 0); // Obstacle Alerts (including ones we send)
  Register("OBM_RESOLVED", 0); // Delete the obstacle from Neptune

  // $MOMIS
  Register("DEPLOY", 0);
  Register("IVPHELM_ALLSTOP", 0);

  // $MOGOH
  Register("MOOS_MANUAL_OVERRIDE", 0);

  Register("NEPTUNE_SURVEY_VISITED_POINT", 0);
}

//---------------------------------------------------------
// Procedure: handleNinjaEvents

void Neptune::handleNinjaEvents()
{
  for (const string& event : m_ninja.getEvents()) reportEvent(event);
  for (const string& warn : m_ninja.getWarnings()) reportRunWarning(warn);
  for (const string& warn : m_ninja.getRetractions()) retractRunWarning(warn);
}

//------------------------------------------------------------
// Procedure: buildReport()

bool Neptune::buildReport() 
{
  m_msgs << "Pending Outbox Messages: " << intToString(send_queue.size()) << endl;
  m_msgs << "Neptune Requested Mailings: " << intToString(forward_mail.size()) << endl;

  m_msgs << endl;

  m_msgs << "Waypt Sequence: " << m_tracking_sequence_id << endl;
  m_msgs << "Remaining Points: " << intToString(points.size()) << " (" << points.get_spec_pts() << ")" << endl;
  m_msgs << "Helm Allstop: " << m_allstop_val << endl;

  m_msgs << endl;

  handleNinjaEvents();
  m_msgs << "SockNinja Report: " << endl;
  for (const string& line : m_ninja.getSummary(false)) m_msgs << "  " << line << endl;
  m_msgs << endl;

  return(true);
}
