/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: AnnounceMail.cpp                                        */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "AnnounceMail.h"
#include "NodeMessage.h"

using namespace std;

//---------------------------------------------------------
// Constructor

AnnounceMail::AnnounceMail()
{
}

//---------------------------------------------------------
// Destructor

AnnounceMail::~AnnounceMail()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool AnnounceMail::OnNewMail(MOOSMSG_LIST &NewMail)
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

    auto pos = m_broadcast_mails.find(key);
    if(pos != m_broadcast_mails.end()) {
      NodeMessage node_message;

      node_message.setSourceApp(GetAppName());
      node_message.setSourceNode(m_Comms.GetCommunityName());
      node_message.setDestNode("all");
      node_message.setVarName(key);
      if (msg.IsDouble()) {
        node_message.setDoubleVal(msg.GetDouble());
      } else {
        node_message.setStringVal(msg.GetAsString());
      }

      Notify("NODE_MESSAGE_LOCAL", node_message.getSpec());
      reportEvent("Forwarded " + key);
    }

     else if(key != "APPCAST_REQ") // handled by AppCastingMOOSApp
       reportRunWarning("Unhandled Mail: " + key);
   }

   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool AnnounceMail::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool AnnounceMail::Iterate()
{
  AppCastingMOOSApp::Iterate();
  // Do your thing here!
  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool AnnounceMail::OnStartUp()
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
    if(param == "broadcast") {
      vector<string> mails = parseString(value, ",");
      for (string mail : mails) {
        m_broadcast_mails.insert(removeWhite(mail));
        Register(mail);
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

void AnnounceMail::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  // Register("FOOBAR", 0);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool AnnounceMail::buildReport()
{
  m_msgs << "============================================" << endl;
  m_msgs << "File:                                       " << endl;
  m_msgs << "============================================" << endl;

  ACTable actab(4);
  actab << "Alpha | Bravo | Charlie | Delta";
  actab.addHeaderLines();
  actab << "one" << "two" << "three" << "four";
  m_msgs << actab.getFormattedString();

  return(true);
}




