/************************************************************/
/*    NAME: Conlan Cesar                                              */
/*    ORGN: MIT                                             */
/*    FILE: PrimeFactor.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include <cstdlib>
#include "MBUtils.h"
#include "PrimeFactor.h"
#include "PrimeFactorEntry.h"

using namespace std;

uint64_t m_orig_number;
unsigned int m_prime_order_number = 0;
unsigned int m_prime_order_delivery = 0;
list<PrimeFactorEntry> order_queue;

//---------------------------------------------------------
// Constructor

PrimeFactor::PrimeFactor()
{
}

//---------------------------------------------------------
// Destructor

PrimeFactor::~PrimeFactor()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool PrimeFactor::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;

  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

    if (msg.GetKey() == "NUM_VALUE") {
      m_prime_order_number++;
      uint64_t numb = atoi(msg.GetString().c_str());
      order_queue.push_back(PrimeFactorEntry(numb, m_prime_order_number, MOOSTime()));
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

std::string my_to_string(int64_t i) {
  std::stringstream ss;
  ss << i;
  return ss.str();
}

string PrimeFactor::GetFormattedString(PrimeFactorEntry i, string username) {
  m_prime_order_delivery++;

  string st = "orig="+my_to_string(i.GetTargetNumber())+",delivered="+my_to_string(m_prime_order_delivery)+",received="+my_to_string(i.GetOrderNumber())+",user="+username+",primes=";
  for (unsigned int j = 0; j < i.GetCalculatedPrimeFactors().size(); ++j) {
    st += (to_string(i.GetCalculatedPrimeFactors().at(j)) + ":");
  }
  st = st.substr(0, st.size()-1); // Remove the trailling ':'
  st += ",solve_time="+to_string(abs(i.GetStartTime() - MOOSTime()));
  return st;
}


//---------------------------------------------------------
// Procedure: OnConnectToServer

bool PrimeFactor::OnConnectToServer()
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

bool PrimeFactor::Iterate()
{
  if (!order_queue.empty()) {
    list<PrimeFactorEntry>::iterator p;
    for(p=order_queue.begin(); p!=order_queue.end();) {
      PrimeFactorEntry entry = *p;
      entry.ContinueCalculations(999);
      m_Comms.Notify("PLEASE", entry.GetTargetNumber());
      if (entry.IsFinished()) {
        m_Comms.Notify("PRIME_RESULT", GetFormattedString(entry, "conlanc"));
        p = order_queue.erase(p);
      }
      p++;
    }
  }
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool PrimeFactor::OnStartUp()
{
  list<string> sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    list<string>::iterator p;
    for(p=sParams.begin(); p!=sParams.end(); p++) {
      string original_line = *p;
      string param = stripBlankEnds(toupper(biteString(*p, '=')));
      string value = stripBlankEnds(*p);

      if(param == "help") {
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

void PrimeFactor::RegisterVariables()
{
  Register("NUM_VALUE", 0);
}

