#ifndef RVIZ_AUTO_SENSOR_PANEL__AUTO_DISPLAY_FACTORY_HPP_
#define RVIZ_AUTO_SENSOR_PANEL__AUTO_DISPLAY_FACTORY_HPP_

#include <string>

#include "rviz_common/display.hpp"
#include "rviz_common/visualization_manager.hpp"

namespace rviz_auto_sensor_panel
{

class AutoDisplayFactory
{
public:
  rviz_common::Display * createDisplay(
    rviz_common::VisualizationManager * visualization_manager,
    const std::string & display_type,
    const std::string & topic_name,
    const std::string & message_type) const;

private:
  QString buildDisplayName(const std::string & display_type, const std::string & topic_name) const;
};

}  // namespace rviz_auto_sensor_panel

#endif  // RVIZ_AUTO_SENSOR_PANEL__AUTO_DISPLAY_FACTORY_HPP_
