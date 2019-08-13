/****************************************************************/
/*   NAME: Conlan Cesar                                         */
/*   ORGN: MIT Cambridge MA                                     */
/*   FILE: RaspiMon.h                                           */
/*   DATE: Summer 2019                                          */
/****************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "RaspiMon.h"

using namespace std;

//---------------------------------------------------------
// Constructor

RaspiMon::RaspiMon()
{
  temperatureWarnThreshold = 70;
  reportedVcgenFailure = false;
  reportedTemperatureThreshSurpassed = false;
}

//---------------------------------------------------------
// Destructor

RaspiMon::~RaspiMon()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool RaspiMon::OnNewMail(MOOSMSG_LIST &NewMail)
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

     if(key == "FOO") 
       cout << "great!";

     else if(key != "APPCAST_REQ") // handled by AppCastingMOOSApp
       reportRunWarning("Unhandled Mail: " + key);
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool RaspiMon::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool RaspiMon::Iterate() {
  AppCastingMOOSApp::Iterate();
  if (!reportedVcgenFailure && !vcCmd.sanityCheck()) {
    reportEvent("VCGenCmd not found, not reporting metrics");
    reportedVcgenFailure = true;
  }
  AppCastingMOOSApp::PostReport();

  if (reportedVcgenFailure) { return true; }

  float voltage = vcCmd.getVoltage();
  float temperature = vcCmd.getTemperature();
  long armClockSpeed = vcCmd.getClockSpeed();
  string throttleState = vcCmd.getThrottleHex();

  m_Comms.Notify("SYSTEM_TEMPERATURE", temperature);
  m_Comms.Notify("VIDEOCORE_VOLTAGE", voltage);
  m_Comms.Notify("THROTTLE_STATE", throttleState);
  m_Comms.Notify("ARM_CLOCK_SPEED", armClockSpeed);

  if (temperatureWarnThreshold != -1) {
    const string surpassWarningString = "Temperature surpassed limit of " + floatToString(temperatureWarnThreshold);

    if (temperature >= temperatureWarnThreshold && !reportedTemperatureThreshSurpassed) {
      reportRunWarning(surpassWarningString);
      reportedTemperatureThreshSurpassed = true;
    } else if (temperature < temperatureWarnThreshold && reportedTemperatureThreshSurpassed) {
      retractRunWarning(surpassWarningString);
      reportEvent("Temperature was above " + floatToString(temperatureWarnThreshold) +
                  ", but now it's below. Recalled Run Warning");
      reportedTemperatureThreshSurpassed = false;
    }
  }

  if (tempHistory.size() >= 60 * 10 * m_time_warp) { // Store 10 mins worth of samples
    tempHistory.pop_back();
  }
  tempHistory.push_front(temperature);

  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool RaspiMon::OnStartUp()
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
    if(param == "temperature_threshold") {
      temperatureWarnThreshold = atof(value.c_str());
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

void RaspiMon::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  // Register("FOOBAR", 0);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool RaspiMon::buildReport()
{
  m_msgs << "VCGenCmd Found: " << std::boolalpha << !reportedVcgenFailure << std::noboolalpha << endl;
  if (reportedVcgenFailure)
    return true; // Don't get the rest of the information, it isn't there to get

  float temp1m = 0;
  float temp5m = 0;
  float temp10m = 0;

  for(size_t a = 0; a < tempHistory.size(); a++) {
    float temp = tempHistory.at(a);
    if (a < 60 * 1 * m_time_warp && tempHistory.size() > 60 * 1 * m_time_warp) temp1m += temp;
    if (a < 60 * 5 * m_time_warp && tempHistory.size() > 60 * 5 * m_time_warp) temp5m += temp;
    if (a < 60 * 10 * m_time_warp && tempHistory.size() > 60 * 10 * m_time_warp) temp10m += temp;
  }

  m_msgs << endl;
  m_msgs << "Temperature: " << vcCmd.getTemperature() << "°C [ " << temp1m << " | " << temp5m << " | " << temp10m << " ]" << endl;
  m_msgs << "GPU Voltage: " << vcCmd.getVoltage() << "V" << endl;
  m_msgs << "ARM Clock:   " << vcCmd.getClockSpeed() << "Hz" << endl;
  m_msgs << endl;

  // Key here: https://github.com/raspberrypi/documentation/blob/JamesH65-patch-vcgencmd-vcdbg-docs/raspbian/applications/vcgencmd.md#get_throttled
  // Explanation: https://raspberrypi.org/forums/viewtopic.php?f=63&t=147781&start=50#p972790

  string binaryThrottle = vcCmd.getThrottleBinary();

  m_msgs << "Throttle State: 0x" << vcCmd.getThrottleHex() << " (" << binaryThrottle << ")" << endl;
  m_msgs << endl;

  m_msgs << "CURRENTLY" << endl;
  m_msgs << "ARM Frequency Capped: " << binaryThrottle.at(1) << endl;
  m_msgs << "Temperature Limited:  " << binaryThrottle.at(3) << endl;
  m_msgs << "Under-voltage:  " << binaryThrottle.at(0) << endl;
  m_msgs << "Turbo Disabled: " << binaryThrottle.at(2) << endl;
  m_msgs << endl;

  m_msgs << "PREVIOUSLY " << endl;
  m_msgs << "ARM Frequency Capped: " << binaryThrottle.at(18) << endl;
  m_msgs << "Temperature Limited:  " << binaryThrottle.at(19) << endl;
  m_msgs << "Under-voltage:  " << binaryThrottle.at(16) << endl;
  m_msgs << "Turbo Disabled: " << binaryThrottle.at(17) << endl;
  m_msgs << endl;

  return(true);
}
