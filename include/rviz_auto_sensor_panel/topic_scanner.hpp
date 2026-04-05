#ifndef RVIZ_AUTO_SENSOR_PANEL__TOPIC_SCANNER_HPP_
#define RVIZ_AUTO_SENSOR_PANEL__TOPIC_SCANNER_HPP_

#include <memory>
#include <vector>

#include "rclcpp/rclcpp.hpp"

#include "rviz_auto_sensor_panel/sensor_classifier.hpp"
#include "rviz_auto_sensor_panel/types.hpp"

namespace rviz_auto_sensor_panel
{

class TopicScanner
{
public:
  explicit TopicScanner(SensorClassifier classifier);

  std::vector<DiscoveredTopic> scan(const rclcpp::Node::SharedPtr & node) const;

private:
  SensorClassifier classifier_;
};

}  // namespace rviz_auto_sensor_panel

#endif  // RVIZ_AUTO_SENSOR_PANEL__TOPIC_SCANNER_HPP_
