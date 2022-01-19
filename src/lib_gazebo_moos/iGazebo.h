#ifndef Gazebo_HEADER
#define Gazebo_HEADER

#include <MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h>
#include <MOOS/libMOOSGeodesy/MOOSGeodesy.h>
#include "NodeRecord.h"

class iGazebo : public AppCastingMOOSApp
{
 public:
   iGazebo();
   ~iGazebo();

 public: // Standard MOOSApp functions to overload
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload
   bool buildReport();
   void registerVariables();
};
#endif
