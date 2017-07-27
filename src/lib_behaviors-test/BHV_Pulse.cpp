/************************************************************/
/*    NAME: Conlan Cesar                                              */
/*    ORGN: MIT                                             */
/*    FILE: BHV_Pulse.cpp                                    */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include <cstdlib>
#include "MBUtils.h"
#include "XYRangePulse.h"
#include "BuildUtils.h"
#include "BHV_Pulse.h"

using namespace std;

//---------------------------------------------------------------
// Constructor

BHV_Pulse::BHV_Pulse(IvPDomain domain) :
  IvPBehavior(domain)
{
  // Provide a default behavior name
  IvPBehavior::setParam("name", "defaultname");

  // Declare the behavior decision space
  m_domain = subDomain(m_domain, "course,speed");

  // Add any variables this behavior needs to subscribe for
  addInfoVars("NAV_X, NAV_Y");
  addInfoVars("WPT_INDEX", "no_warning");

  m_wpt_index = 0;
  m_wpt_trigger_time = 0;
  m_waiting = false;

  m_pulse_radius = 20;
  m_pulse_time = 4;
}

//---------------------------------------------------------------
// Procedure: setParam()

bool BHV_Pulse::setParam(string param, string val)
{
  // Convert the parameter to lower case for more general matching
  param = tolower(param);

  // Get the numerical value of the param argument for convenience once
  double double_val = atof(val.c_str());
  
  if((param == "pulse_range") && isNumber(val)) {
    m_pulse_radius = double_val;
    return(true);
  }
  else if (param == "pulse_duration") {
    m_pulse_time = double_val;
    return(true);
  }

  // If not handled above, then just return false;
  return(false);
}

//---------------------------------------------------------------
// Procedure: onSetParamComplete()
//   Purpose: Invoked once after all parameters have been handled.
//            Good place to ensure all required params have are set.
//            Or any inter-param relationships like a<b.

void BHV_Pulse::onSetParamComplete()
{
}

//---------------------------------------------------------------
// Procedure: onHelmStart()
//   Purpose: Invoked once upon helm start, even if this behavior
//            is a template and not spawned at startup

void BHV_Pulse::onHelmStart()
{
}

//---------------------------------------------------------------
// Procedure: onIdleState()
//   Purpose: Invoked on each helm iteration if conditions not met.

void BHV_Pulse::onIdleState()
{
}

//---------------------------------------------------------------
// Procedure: onCompleteState()

void BHV_Pulse::onCompleteState()
{
}

//---------------------------------------------------------------
// Procedure: postConfigStatus()
//   Purpose: Invoked each time a param is dynamically changed

void BHV_Pulse::postConfigStatus()
{
}

//---------------------------------------------------------------
// Procedure: onIdleToRunState()
//   Purpose: Invoked once upon each transition from idle to run state

void BHV_Pulse::onIdleToRunState()
{
}

//---------------------------------------------------------------
// Procedure: onRunToIdleState()
//   Purpose: Invoked once upon each transition from run to idle state

void BHV_Pulse::onRunToIdleState()
{
}

//---------------------------------------------------------------
// Procedure: onRunState()
//   Purpose: Invoked each iteration when run conditions have been met.

IvPFunction* BHV_Pulse::onRunState()
{
  // Part 1: Build the IvP function
  IvPFunction *ipf = 0;

  bool wptOK;
  int currWpt = (int) getBufferDoubleVal("WPT_INDEX", wptOK);

  if (wptOK && currWpt != m_wpt_index) {
    m_wpt_index = currWpt;
    m_waiting = true;
    m_wpt_trigger_time = getBufferCurrTime() + 5; // Trigger in 5 seconds
  }

  checkSendPulse();

  // Part N: Prior to returning the IvP function, apply the priority wt
  // Actual weight applied may be some value different than the configured
  // m_priority_wt, depending on the behavior author's insite.
  if(ipf)
    ipf->setPWT(m_priority_wt);

  return(ipf);
}

void BHV_Pulse::checkSendPulse() {
  if (m_waiting && getBufferCurrTime() > m_wpt_trigger_time) {
    bool navOK = true;
    XYRangePulse pulse;
    pulse.set_x(getBufferDoubleVal("NAV_X", navOK));
    if (navOK) pulse.set_y(getBufferDoubleVal("NAV_Y", navOK));
    pulse.set_label("bhv_pulse");
    pulse.set_rad(m_pulse_radius);
    pulse.set_duration(m_pulse_time);
    pulse.set_time(m_wpt_trigger_time);
    pulse.set_color("edge", "yellow");
    pulse.set_color("fill", "yellow");

    if (navOK) {
      m_waiting = false;
      string spec = pulse.get_spec();
      postMessage("VIEW_RANGE_PULSE", spec);
    }
  }
}