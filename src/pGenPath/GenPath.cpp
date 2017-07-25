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

    if (msg.GetKey() == "NAV_X") lastX = msg.GetDouble();
    if (msg.GetKey() == "NAV_Y") lastY = msg.GetDouble();

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

  /*
  for (int i = 0; pSize > i; i++) {
    unsigned int closestIndex = getClosestPointIndex(points.closest_vertex(), points.get_vy(0), points);
    previousX = points.get_vx(closestIndex);
    previousY = points.get_vy(closestIndex);
    newPoints.add_vertex(previousX, previousY);
    points.delete_vertex(closestIndex);
  }
  */

  for (int i = 0; pSize > i; i++) {
    unsigned int closestIndex = points.closest_vertex(previousX, previousY);
    previousX = points.get_vx(closestIndex);
    previousY = points.get_vy(closestIndex);
    newPoints.add_vertex(previousX, previousY);
    points.delete_vertex(closestIndex);
  }
  newPoints.delete_vertex(0, 0); // Not sure why, but for some reason the point 0,0 is added along the line

  std::string points_string = "points=";
  points_string += newPoints.get_spec();
  reportEvent("Sending ship with " + points_string);
  m_Comms.Notify("SURVEY_POINTS", points_string);
  return(true);
}

double GenPath::pointDistance(double x1, double y1, double x2, double y2) {
  return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

unsigned int GenPath::getClosestPointIndex(double x, double y, XYSegList p) {
  double min = INFINITY;
  unsigned int indexOfMin = UINT_MAX;

  for (unsigned int i = 0; i < p.size(); ++i) {
    if (p.get_vx(i) == x && p.get_vy(i) == y) { // Don't calculate self
      p.delete_vertex(x, y);
    } else {
      double dist = pointDistance(p.get_vx(i), p.get_vy(i), x, y); // Get distance between two points
      if (dist < min) {
        min = dist; // If distance is less than minimum, set new minimum
        indexOfMin = points.closest_vertex(p.get_vx(i), p.get_vy(i)); // Get Index of that minimum
      }
    }
  }
  return indexOfMin; // Return index of found
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

  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool GenPath::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();
  list<string> sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    list<string>::iterator p;
    for(p=sParams.begin(); p!=sParams.end(); p++) {
      string original_line = *p;
      string param = stripBlankEnds(toupper(biteString(*p, '=')));
      string value = stripBlankEnds(*p);
      
      if(param == "FOO") {
        //handled
      }
      else if(param == "BAR") {
        //handled
      }
    }
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
}

bool GenPath::buildReport()
{
  m_msgs << "Assigned " + to_string(points.size()) + " points" << endl;
  return(true);
}
