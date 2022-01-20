#ifndef Gazebo_HEADER
#define Gazebo_HEADER

#include <ignition/msgs/twist.pb.h>
#include <ignition/transport/Node.hh>
#include <sdf/sdf.hh>
#include <ignition/common/Console.hh>

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
    std::function<void(const ignition::msgs::Any&, const ignition::transport::MessageInfo&)> ignitionCallbackFactory(const std::string moos_key);

  private:
    // Set up transport
    ignition::transport::Node node;
    std::map<std::string, ignition::transport::Node::Publisher> mapping_map;
};
#endif
