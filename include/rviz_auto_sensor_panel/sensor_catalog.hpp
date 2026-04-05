#ifndef RVIZ_AUTO_SENSOR_PANEL__SENSOR_CATALOG_HPP_
#define RVIZ_AUTO_SENSOR_PANEL__SENSOR_CATALOG_HPP_

#include <map>
#include <string>
#include <vector>

#include "rviz_auto_sensor_panel/types.hpp"

namespace rviz_auto_sensor_panel
{

class SensorCatalog
{
public:
  void update(const std::vector<DiscoveredTopic> & latest_topics);
  std::vector<DiscoveredTopic> allTopics() const;
  std::map<SensorCategory, std::vector<DiscoveredTopic>> groupedTopics() const;

private:
  std::map<std::string, DiscoveredTopic> topics_by_name_;
};

}  // namespace rviz_auto_sensor_panel

#endif  // RVIZ_AUTO_SENSOR_PANEL__SENSOR_CATALOG_HPP_
