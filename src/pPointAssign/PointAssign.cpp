/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: PointAssign.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include <XYSegList.h>
#include "ACTable.h"
#include "PointAssign.h"

using namespace std;

std::vector<std::string> vehicles;
bool assignByRegion = false;
unsigned long current_vehicle_index = 0;
XYSegList posList;

//---------------------------------------------------------
// Constructor

PointAssign::PointAssign()
{
}

//---------------------------------------------------------
// Destructor

PointAssign::~PointAssign()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool PointAssign::OnNewMail(MOOSMSG_LIST &NewMail)
{
    AppCastingMOOSApp::OnNewMail(NewMail);

    MOOSMSG_LIST::iterator p;
    for(p=NewMail.begin(); p!=NewMail.end(); p++) {
        CMOOSMsg &msg = *p;
        string key    = msg.GetKey();

        if (key == "VISIT_POINT") {

            if (toupper(msg.GetString()) == toupper("firstpoint")) {
                true;
            } else if (toupper(msg.GetString()) == toupper("lastpoint")) {
              sendShips();
              m_Comms.Notify("VISIT_POINT_ALL", "lastpoint");
            } else {
                std::string xpos = tokStringParse(msg.GetString(), "x", ',', '='); // See http://oceanai.mit.edu/ivpman/pmwiki/pmwiki.php?n=Help.StringParsing
                float xcoord = std::stof(xpos.c_str());

                std::string ypos = tokStringParse(msg.GetString(), "y", ',', '=');
                float ycoord = std::stof(ypos.c_str());

                std::string _id = tokStringParse(msg.GetString(), "id", ',', '=');

                posList.add_vertex(xcoord, ycoord);
            }
        } else if(key == "FOO") {
            cout << "great!";
        } else if(key != "APPCAST_REQ") { // handled by AppCastingMOOSApp
            reportRunWarning("Unhandled Mail: " + key);
        }

#if 0 // Keep these around just for template
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

void PointAssign::sendShips() {
    if (assignByRegion) {
        regionNotify();
    } else {
        orderNotify();
    }
}

void PointAssign::orderNotify() {
    for (unsigned int i = 0; posList.size() > i; i++) {
        if (current_vehicle_index > vehicles.size() - 1) current_vehicle_index = 0;
        m_Comms.Notify("VISIT_POINT_" + vehicles.at(current_vehicle_index),
                       "x=" + to_string(posList.get_vx(i)) + ",y=" + to_string(posList.get_vy(i)));
      current_vehicle_index++;
    }
}

void PointAssign::regionNotify() {
    for (unsigned int i = 0; posList.size() > i; i++) {
      reportConfigWarning("x=" + to_string(posList.get_vx(i)) + ",y=" + to_string(posList.get_vy(i)));
        if (posList.get_vx(i) > posList.get_avg_x()){
            m_Comms.Notify("VISIT_POINT_" + vehicles.at(0),
                           "x=" + to_string(posList.get_vx(i)) + ",y=" + to_string(posList.get_vy(i)));
        } else {
            m_Comms.Notify("VISIT_POINT_" + vehicles.at(1),
                           "x=" + to_string(posList.get_vx(i)) + ",y=" + to_string(posList.get_vy(i)));
        }
    }
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool PointAssign::OnConnectToServer()
{
    registerVariables();
    return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool PointAssign::Iterate()
{
    AppCastingMOOSApp::Iterate();
    m_Comms.Notify("PointAssignReady", "true"); // Notify the timer script that we are ready for input
    AppCastingMOOSApp::PostReport();
    return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool PointAssign::OnStartUp()
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
        if(param == "vname") {
            vehicles.push_back(toupper(value));
            handled = true;
        } else if(param == "assign_by_region") {
            if (value == "true") assignByRegion = true;
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

void PointAssign::registerVariables()
{
    AppCastingMOOSApp::RegisterVariables();
    Register("VISIT_POINT", 0);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool PointAssign::buildReport()
{
    m_msgs << "============================================ \n";
    m_msgs << "File:                                        \n";
    m_msgs << "============================================ \n";

    m_msgs << "Assigning by Region: " << to_string(assignByRegion) << endl;
    m_msgs << "Received a total of " << posList.size() << " points" << endl;
    //m_msgs << "Next assignment to: " << vehicles.at(currentVehicleIndex);
    //m_msgs << "Assigned " + to_string(num_points) + " points to " + to_string(vehicles.size()) + " vehicles" << endl;

    return(true);
}




