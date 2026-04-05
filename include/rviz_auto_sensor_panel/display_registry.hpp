#ifndef RVIZ_AUTO_SENSOR_PANEL__DISPLAY_REGISTRY_HPP_
#define RVIZ_AUTO_SENSOR_PANEL__DISPLAY_REGISTRY_HPP_

#include <map>
#include <string>

#include "rviz_common/display.hpp"

#include "rviz_auto_sensor_panel/types.hpp"

namespace rviz_auto_sensor_panel
{

class DisplayRegistry
{
public:
  bool hasDisplay(const std::string & topic_name) const;
  rviz_common::Display * getDisplay(const std::string & topic_name) const;
  void registerDisplay(const ManagedDisplay & managed_display, rviz_common::Display * display);
  void setEnabled(const std::string & topic_name, bool enabled);
  bool isEnabled(const std::string & topic_name) const;

private:
  struct Entry
  {
    ManagedDisplay managed_display;
    rviz_common::Display * display{nullptr};
  };

  std::map<std::string, Entry> entries_;
};

}  // namespace rviz_auto_sensor_panel

#endif  // RVIZ_AUTO_SENSOR_PANEL__DISPLAY_REGISTRY_HPP_
