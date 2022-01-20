#include <iterator>
#include <functional>

#include "MBUtils.h"
#include "ACTable.h"
#include "iGazebo.h"

using namespace std;

//---------------------------------------------------------
// Constructor

iGazebo::iGazebo()
{
}

//---------------------------------------------------------
// Destructor

iGazebo::~iGazebo()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool iGazebo::OnNewMail(MOOSMSG_LIST &NewMail)
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

    ignition::transport::Node::Publisher& ignpub = mapping_map.find(key)->second;
    if (ignpub) {
      // We are registered for a mapping, and it was posted to by MOOS, process it
      if (msg.IsDouble()) {
        ignition::msgs::Double v;
        v.set_data(msg.GetDouble());
        ignpub.Publish(v);
      } else {
        // We can't handle all of the ignition::msgs types now, so convert to string
        // If there is a need / interest, we may be able to fix this
        ignition::msgs::StringMsg s;
        s.set_data(msg.GetAsString());
        ignpub.Publish(s);
      }
    }

     else if(key != "APPCAST_REQ") // handled by AppCastingMOOSApp
       reportRunWarning("Unhandled Mail: " + key);
   }

   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool iGazebo::OnConnectToServer()
{
  registerVariables();

  // if we want to read from SDF, this is useful. However, we're probably going to use MOOS for this bit
  /*
  // Get parameters from SDF file
  auto sdf = sdf::readFile("/dev/null");
  auto plugin = sdf->Root()->GetElement("world")->GetElement("plugin");
  auto twistTopic = plugin->Get<std::string>("twist_arrows", "/cmd_vel").first;
  */


  return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool iGazebo::Iterate()
{
  AppCastingMOOSApp::Iterate();
  // Do your thing here!
  AppCastingMOOSApp::PostReport();
  return(true);
}

// Callback for new ignition messages
std::function<void(const ignition::msgs::Any&, const ignition::transport::MessageInfo&)> iGazebo::ignitionCallbackFactory(const std::string moos_key) {
  auto callback = [&](const ignition::msgs::Any &_msg, const ignition::transport::MessageInfo &_info) mutable -> void {
    // TODO have srcAux be the notifying ignition node name
    if (_info.Type() == "ignition::msgs::Double") {
      // Type is double, post it as such
      Notify(moos_key, _msg.double_value());
    } else if (_info.Type() == "ignition::msgs::StringMsg") {
      // Type is string, post it as such
      // todo check this -- is double_value accurate?
      Notify(moos_key, _msg.string_value());
    } else {
      // Type is any, post the debug string
      // Consider using https://ignitionrobotics.org/api/gazebo/6.0/triggeredpublisher.html
      reportRunWarning("Notifying unimplemented type " + _info.Type() + " from ignition topic " + _info.Topic());
      Notify(moos_key, _msg.string_value());
    }
  };

  return callback;
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool iGazebo::OnStartUp()
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
    if(param == "moos_origin") {
      string moos_key = biteStringX(line, ':');
      string ign_key = line;
      mapping_map.insert(pair<std::string, ignition::transport::Node::Publisher>(toupper(moos_key), node.Advertise<ignition::msgs::Any>(ign_key)));
      Register(moos_key);
      handled = true;
    } else if (param == "ign_origin") {
      string ign_key = biteStringX(line, ':');
      string moos_key = line;
      bool subscribedSuccessfully = false; //TODO node.Subscribe(ign_key, (ignitionCallbackFactory(moos_key)), this);
      if (!subscribedSuccessfully) {
        reportUnhandledConfigWarning("Unable to subscribe to " + ign_key);
      }
      handled = subscribedSuccessfully;
    }

    if(!handled)
      reportUnhandledConfigWarning(orig);

  }

  registerVariables();
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void iGazebo::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  // Register("FOOBAR", 0);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool iGazebo::buildReport()
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




