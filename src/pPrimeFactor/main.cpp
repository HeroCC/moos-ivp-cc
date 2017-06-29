/************************************************************/
/*    NAME: Conlan Cesar                                              */
/*    ORGN: MIT                                             */
/*    FILE: main.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <string>
#include "MBUtils.h"
#include "ColorParse.h"
#include "PrimeFactor.h"
#include "PrimeFactorEntry.h"
#include "PrimeFactor_Info.h"

using namespace std;

int main(int argc, char *argv[])
{
  string mission_file;
  string run_command = argv[0];

  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    if((argi=="-v") || (argi=="--version") || (argi=="-version"))
      showReleaseInfoAndExit();
    else if((argi=="-e") || (argi=="--example") || (argi=="-example"))
      showExampleConfigAndExit();
    else if((argi == "-h") || (argi == "--help") || (argi=="-help"))
      showHelpAndExit();
    else if((argi == "-i") || (argi == "--interface"))
      showInterfaceAndExit();
    else if(strEnds(argi, ".moos") || strEnds(argi, ".moos++"))
      mission_file = argv[i];
    else if(strBegins(argi, "--alias="))
      run_command = argi.substr(8);
    else if(strBegins(argi, "--testPrime=")) {
      PrimeFactorEntry entry = PrimeFactorEntry(std::atoi(argi.substr(12).c_str()), 1, 0);
      entry.ContinueCalculations(999);
      for (auto &i : entry.GetCalculatedPrimeFactors()) {
        cout << "thingie: " << to_string(i) << endl;
      }
      cout << "finished: " << entry.IsFinished() << endl;
      return 0;
    } else if(i==2)
      run_command = argi;
  }
  
  if(mission_file == "")
    showHelpAndExit();

  cout << termColor("green");
  cout << "pPrimeFactor launching as " << run_command << endl;
  cout << termColor() << endl;

  PrimeFactor PrimeFactor;

  PrimeFactor.Run(run_command.c_str(), mission_file.c_str());
  
  return(0);
}

