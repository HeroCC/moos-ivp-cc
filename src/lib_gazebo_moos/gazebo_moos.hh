
#ifndef MOOS_GAZEBO_PLUGIN_HH_
#define MOOS_GAZEBO_PLUGIN_HH_

#include <ignition/gazebo/System.hh>

namespace gazebo_moos
{
  class MessageMap:
    // This class is a system.
    public ignition::gazebo::System,
    public ignition::gazebo::ISystemPreUpdate,
    public ignition::gazebo::ISystemUpdate,
    public ignition::gazebo::ISystemPostUpdate
  {
    public: MessageMap();
    public: ~MessageMap() override;
    public: void PreUpdate(const ignition::gazebo::UpdateInfo &_info, ignition::gazebo::EntityComponentManager &_ecm) override;
    public: void Update(const ignition::gazebo::UpdateInfo &_info, ignition::gazebo::EntityComponentManager &_ecm) override;
    public: void PostUpdate(const ignition::gazebo::UpdateInfo &_info, const ignition::gazebo::EntityComponentManager &_ecm) override;
  };
}
#endif
