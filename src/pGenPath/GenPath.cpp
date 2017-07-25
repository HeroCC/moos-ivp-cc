/************************************************************/
/*    NAME: Conlan Cesar                                              */
/*    ORGN: MIT                                             */
/*    FILE: GenPath.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "GenPath.h"

using namespace std;

//---------------------------------------------------------
// Constructor

GenPath::GenPath()
{
}

//---------------------------------------------------------
// Destructor

GenPath::~GenPath()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool GenPath::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);
  MOOSMSG_LIST::iterator p;
   
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

    if (msg.GetKey() == "NAV_X") {
      lastX = msg.GetDouble();
    }
    if (msg.GetKey() == "NAV_Y") {
      lastY = msg.GetDouble();
    }
    if (msg.GetKey() == "GENPATH_REGENERATE" && msg.GetString() == "true") {
      points = neededPoints;
      reportEvent("Regenerating with " + to_string(neededPoints.size()) + " Ship points");
      sendShip();
    }
    if (msg.GetKey() == "VISIT_POINT") {
      if (msg.GetString() == "lastpoint") {
        sendShip();
      } else {
          std::string xpos = tokStringParse(msg.GetString(), "x", ',', '=');
          std::string ypos = tokStringParse(msg.GetString(), "y", ',', '=');

          float xcoord = std::stof(xpos.c_str());
          float ycoord = std::stof(ypos.c_str());

          points.add_vertex(xcoord, ycoord);
      }
    } 

#if 0 // Keep these around just for template
    string key   = msg.GetKey();
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString(); 
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif
   }
	
   return(true);
}

bool GenPath::sendShip() {
  XYSegList newPoints;

  unsigned int pSize = points.size();

  double previousX = lastX;
  double previousY = lastY;

  for (int i = 0; pSize > i; i++) {
    unsigned int closestIndex = points.closest_vertex(previousX, previousY);
    previousX = points.get_vx(closestIndex);
    previousY = points.get_vy(closestIndex);
    newPoints.add_vertex(previousX, previousY);
    points.delete_vertex(closestIndex);
  }
  newPoints.delete_vertex(0, 0); // Not sure why, but for some reason the point 0,0 is added along the line

  std::string points_string = "points=";
  if (newPoints.size() != 0) {
    points_string += newPoints.get_spec();
    reportEvent("Sending ship with " + points_string);
    m_Comms.Notify("SURVEY_POINTS", points_string);
  } else {
    reportEvent("No more points to assign!");
    m_Comms.Notify("SURVEY", "false"); // Stop Surveying
    m_Comms.Notify("RETURN", "true");
  }
  points = newPoints;
  neededPoints = points;
  return(true);
}

double GenPath::pointDistance(double x1, double y1, double x2, double y2) {
  return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

bool GenPath::checkPointRange(double x1, double y1, double x2, double y2) {
  //reportEvent(to_string(x1) + " compared to " + to_string(x2) + " " + to_string(y1) + " compared to " + to_string(y2));
  return pointDistance(x1, x2, y1, y2) <= maxRadius;
}

void GenPath::removePointIfInRange() {
  for (unsigned int i = 0; i < neededPoints.size(); ++i) {
    double x = neededPoints.get_vx(i);
    double y = neededPoints.get_vy(i);
    double currentX = lastX;
    double currentY = lastY;
    if (checkPointRange(x, currentX, y, currentY)) {
      reportEvent("Deleted point:" + to_string(x) + " " + to_string(y));
      neededPoints.delete_vertex(x, y);
    }
  }
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool GenPath::OnConnectToServer()
{
  // register for variables here
  // possibly look at the mission file?
  // m_MissionReader.GetConfigurationParam("Name", <string>);
  // m_Comms.Register("VARNAME", 0);
  

  RegisterVariables();
  return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool GenPath::Iterate()
{
  AppCastingMOOSApp::Iterate();
  removePointIfInRange();
  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool GenPath::OnStartUp()
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
    string param = biteStringX(line, '=');
    string value = line;

    bool handled = false;
    if(param == "visit_radius") {
      maxRadius = stoi(value);
      handled = true;
    }

    if(!handled)
      reportUnhandledConfigWarning(orig);

  }

  RegisterVariables();
  return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void GenPath::RegisterVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("VISIT_POINT", 0);
  Register("NAV_X", 0);
  Register("NAV_Y", 0);
  Register("GENPATH_REGENERATE", 0);
}

bool GenPath::buildReport()
{
  m_msgs << "Assigned " + to_string(points.size()) + " points" << endl;
  m_msgs << "Max Range: " + to_string(maxRadius) << endl;
  return(true);
}
