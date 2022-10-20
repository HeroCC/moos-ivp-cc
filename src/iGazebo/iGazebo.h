#ifndef Gazebo_HEADER
#define Gazebo_HEADER

#include <gz/transport/Node.hh>
#include <sdf/sdf.hh>
#include <gz/common/Console.hh>
#include <gz/msgs.hh>

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

  private:
    std::function<void(const gz::msgs::Any&, const gz::transport::MessageInfo&)> ignitionCallbackFactory(const std::string moos_key);

  private:
    // Set up transport
    gz::transport::Node node;
    std::map<std::string, gz::transport::Node::Publisher> mapping_map;
};
#endif
