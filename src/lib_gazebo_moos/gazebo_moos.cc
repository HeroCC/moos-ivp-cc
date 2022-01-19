#include "gazebo_moos.hh"

#include <ignition/plugin/Register.hh>

IGNITION_ADD_PLUGIN(
    gazebo_moos::MessageMap,
    ignition::gazebo::System,
    gazebo_moos::MessageMap::ISystemPreUpdate,
    gazebo_moos::MessageMap::ISystemUpdate,
    gazebo_moos::MessageMap::ISystemPostUpdate)

using namespace gazebo_moos;

MessageMap::MessageMap()
{
}

MessageMap::~MessageMap()
{
}

void MessageMap::PreUpdate(const ignition::gazebo::UpdateInfo &_info,
    ignition::gazebo::EntityComponentManager &_ecm)
{
  ignmsg << "MessageMap::PreUpdate" << std::endl;
}

void MessageMap::Update(const ignition::gazebo::UpdateInfo &_info,
    ignition::gazebo::EntityComponentManager &_ecm)
{
  ignmsg << "MessageMap::Update" << std::endl;
}

void MessageMap::PostUpdate(const ignition::gazebo::UpdateInfo &_info,
    const ignition::gazebo::EntityComponentManager &_ecm)
{
  ignmsg << "MessageMap::PostUpdate" << std::endl;
}
