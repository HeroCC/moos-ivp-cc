/****************************************************************/
/*   NAME: Conlan Cesar                                         */
/*   ORGN: MIT Cambridge MA                                     */
/*   FILE: VCGenMon.h                                           */
/*   DATE: Summer 2019                                          */
/****************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "VcGenMon.h"

using namespace std;

//---------------------------------------------------------
// Constructor

VcGenMon::VcGenMon()
{
  temperatureWarnThreshold = 70;
  reportedVcgenFailure = false;
  reportedTemperatureThreshSurpassed = false;
}

//---------------------------------------------------------
// Destructor

VcGenMon::~VcGenMon()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool VcGenMon::OnNewMail(MOOSMSG_LIST &NewMail)
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

bool VcGenMon::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool VcGenMon::Iterate() {
  AppCastingMOOSApp::Iterate();
  if (!reportedVcgenFailure && !vcCmd.sanityCheck()) {
    reportEvent("VCGenCmd not found, not reporting metrics");
    reportedVcgenFailure = true;
  }
  AppCastingMOOSApp::PostReport();

  if (!reportedVcgenFailure) {
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

  }
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool VcGenMon::OnStartUp()
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

void VcGenMon::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  // Register("FOOBAR", 0);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool VcGenMon::buildReport() 
{
  m_msgs << "VCGenCmd Found: " << std::boolalpha << !reportedVcgenFailure << std::noboolalpha << endl;
  if (reportedVcgenFailure)
    return true; // Don't get the rest of the information, it isn't there to get


  // TODO make a [1m, 5m, 10m] average shown
  m_msgs << endl;
  m_msgs << "Temperature: " << vcCmd.getTemperature() << "°C" << endl;
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
