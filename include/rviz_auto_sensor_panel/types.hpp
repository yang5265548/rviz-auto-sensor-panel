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
  PointCloud,
  Navigation,
  Visualization
};

struct DiscoveredTopic
{
  std::string name;
  std::string message_type;
  SensorCategory category{SensorCategory::Unknown};
  bool is_available{true};
  std::size_t publisher_count{0};
  std::string group_key;
  std::string group_label;
  std::string topic_label;
};

struct TopicGroup
{
  std::string key;
  std::string label;
  SensorCategory category{SensorCategory::Unknown};
  std::vector<DiscoveredTopic> topics;
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
