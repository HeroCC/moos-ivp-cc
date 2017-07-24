/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: PointAssign.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "PointAssign.h"

using namespace std;

std::vector<std::string> vehicles;
uint64_t currentVehicleIndex = 0;
bool assignByRegion = false;
int num_assigned_points = 0;

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
                m_Comms.Notify("VISIT_POINT_ALL", "lastpoint");
                reportConfigWarning(msg.GetString());
            } else {
                std::string xpos = tokStringParse(msg.GetString(), "x", ',', '='); // See http://oceanai.mit.edu/ivpman/pmwiki/pmwiki.php?n=Help.StringParsing
                //float xcoord = std::stof(xpos.c_str());

                std::string ypos = tokStringParse(msg.GetString(), "y", ',', '=');
                //float ycoord = std::stof(ypos.c_str());

                std::string _id = tokStringParse(msg.GetString(), "id", ',', '=');
                //int id = std::stoi(_id.c_str());

                assignPoint(xpos, ypos, _id);
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

void PointAssign::assignPoint(string xcoord, string ycoord, string id) {
    num_assigned_points++;
    if (currentVehicleIndex > vehicles.size() - 1) currentVehicleIndex = 0;
    if (!assignByRegion) {
        cout << currentVehicleIndex << " vehicle assignment, :" << num_assigned_points << endl;
        std::string targetVehicle = toupper(vehicles.at(currentVehicleIndex));
        currentVehicleIndex++;
        //reportConfigWarning("VISIT_POINT_" + targetVehicle + "      " "x=" + xcoord + ",y=" + ycoord + ",id=" + id);
        m_Comms.Notify("VISIT_POINT_" + targetVehicle, "x=" + xcoord + ",y=" + ycoord + ",id=" + id);
        reportEvent("VISIT_POINT_" + targetVehicle + "      " "x=" + xcoord + ",y=" + ycoord + ",id=" + id);
    } else {
        reportConfigWarning("ABR enabled");
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
            vehicles.push_back(value);
            reportConfigWarning(value);

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
    m_msgs << "Received a total of " << num_assigned_points << " points" << endl;
    //m_msgs << "Next assignment to: " << vehicles.at(currentVehicleIndex);
    //m_msgs << "Assigned " + to_string(num_points) + " points to " + to_string(vehicles.size()) + " vehicles" << endl;

    return(true);
}




