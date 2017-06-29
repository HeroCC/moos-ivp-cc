/************************************************************/
/*    NAME: Conlan Cesar                                              */
/*    ORGN: MIT                                             */
/*    FILE: PrimeFactorTester.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include <cstdlib>
#include "MBUtils.h"
#include "PrimeFactorTester.h"

using namespace std;

//---------------------------------------------------------
// Constructor

PrimeFactorTester::PrimeFactorTester()
{
}

//---------------------------------------------------------
// Destructor

PrimeFactorTester::~PrimeFactorTester()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool PrimeFactorTester::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;

  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

    if (msg.GetKey() == "PRIME_RESULT") {
      ValidatePrimeString(msg.GetString());
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

bool PrimeFactorTester::ValidatePrimeString(string msg) {
  /*
  string originString = msg.substr(msg.find("=") + 1, msg.find(","));
  uint64_t origin = std::atoi(originString.c_str());
  string primeString = msg.substr(msg.find(":") - 1, msg.size());
  primeString = primeString.substr(0, primeString.find(","));
  std::replace(primeString.begin(), primeString.end(), ':', ' ');

  string buffer; // https://stackoverflow.com/a/5208977/1709894
  stringstream ss(primeString);

  vector<int> values;
  while (ss >> buffer) {
    int number = std::atoi(buffer.c_str()); 
    values.push_back(number);
  }
  */

  vector<string> parsedMsg = parseString(msg, ',');
  uint64_t origin = std::strtoul(parsedMsg[0].substr(0,5).c_str(), NULL, 0);
  vector<string> numbers = parseString(parsedMsg[4].substr(0,7), ':'); // 5th element is primes, remove 'primes='

  uint64_t total = 1;
  for (auto &number : numbers) {
    total *= std::atoi(number.c_str());
  }
  if (total == origin) {
    m_Comms.Notify("PRIME_RESULT_VALID", msg + ",valid=true");
    return true;
  }
  return false;
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool PrimeFactorTester::OnConnectToServer()
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

bool PrimeFactorTester::Iterate()
{
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool PrimeFactorTester::OnStartUp()
{
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

void PrimeFactorTester::RegisterVariables()
{
  Register("PRIME_RESULT", 0);
}

