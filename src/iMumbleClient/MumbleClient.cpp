/************************************************************/
/*    NAME: Conlan Cesar                                    */
/*    ORGN: MIT                                             */
/*    FILE: MumbleClient.cpp                                */
/*    DATE: Summer 2018                                     */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "MumbleClient.h"
#include "mumlib/include/mumlib.hpp"
#include "portaudio.h"

using namespace std;

//---------------------------------------------------------
// Constructor

MumbleClient::MumbleClient()
{
  mumlib::MumlibConfiguration conf;
  //conf.opusEncoderBitrate = 32000;
  mum = new mumlib::Mumlib(this->callbackHandler, conf);
  this->callbackHandler.mum = this->mum;
}

//---------------------------------------------------------
// Destructor

MumbleClient::~MumbleClient()
{
  Pa_Terminate();
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool MumbleClient::OnNewMail(MOOSMSG_LIST &NewMail)
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

     if(key == "SEND_AUDIO")
       m_sendAudio = msg.GetAsString() == "true";

     else if(key != "APPCAST_REQ") // handled by AppCastingMOOSApp
       reportRunWarning("Unhandled Mail: " + key);
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool MumbleClient::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool MumbleClient::Iterate()
{
  AppCastingMOOSApp::Iterate();
  // Do your thing here!
  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool MumbleClient::OnStartUp()
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
    string param = toupper(biteStringX(line, '='));
    string value = line;

    bool handled = true;
    if(param == "START_MAIL_NAME") {
      m_sendAudioKey = value;
      Register(value);
    }
    else if(param == "BAR") {

    } else { handled = false; }

    if(!handled)
      reportUnhandledConfigWarning(orig);

  }


  // Start Mumlib
  thread server_thread([this]() {
    this->mum->connect("localhost", 64738, this->m_host_community, "");
    this->mum->run();
  });

  server_thread.detach();

  registerVariables();

  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void MumbleClient::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register(m_sendAudioKey, .1); // Add a bit of a delay for a debounce
}


//------------------------------------------------------------
// Procedure: buildReport()

bool MumbleClient::buildReport() 
{
  m_msgs << "============================================ \n";
  m_msgs << "Mumble Client                                \n";
  m_msgs << "============================================ \n";
  m_msgs << "                                             \n";

  return(true);
}



