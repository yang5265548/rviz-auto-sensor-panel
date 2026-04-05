#ifndef RVIZ_AUTO_SENSOR_PANEL__TYPES_HPP_
#define RVIZ_AUTO_SENSOR_PANEL__TYPES_HPP_

#include <string>
#include <vector>

namespace rviz_auto_sensor_panel
{

enum class SensorCategory
{
  Unknown,
  Lidar,
  Camera,
  PointCloud
};

struct DiscoveredTopic
{
  std::string name;
  std::string message_type;
  SensorCategory category{SensorCategory::Unknown};
  bool is_available{true};
};

struct ManagedDisplay
{
  std::string topic_name;
  std::string message_type;
  std::string display_type;
  std::string display_name;
  bool is_enabled{false};
};

std::string toString(SensorCategory category);
std::vector<SensorCategory> supportedCategories();

}  // namespace rviz_auto_sensor_panel

#endif  // RVIZ_AUTO_SENSOR_PANEL__TYPES_HPP_
