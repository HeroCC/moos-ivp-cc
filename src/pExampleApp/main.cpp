// This is the CMOOSApp example from the MOOS website documentation
// Included here for convenience
//
// Feb 10th, 2013

#include "MOOS/libMOOS/App/MOOSApp.h"
#include "ExampleApp.h"

int main(int argc, char *argv[]) 
{
  //here we do some command line parsing ...
  MOOS::CommandLineParser P(argc, argv);
 
  //mission file could be first free parameter
  std::string mission_file = P.GetFreeParameter(0, "Mission.moos");
  
  //app name can be the second free parameter
  std::string app_name = P.GetFreeParameter(1, "ExampleApp"); 
  
  ExampleApp App;
  App.Run(app_name, mission_file, argc, argv);
  
  return(0);
}

