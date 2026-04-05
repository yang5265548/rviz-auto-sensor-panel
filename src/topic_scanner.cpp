#include "rviz_auto_sensor_panel/topic_scanner.hpp"

#include <algorithm>

namespace rviz_auto_sensor_panel
{

TopicScanner::TopicScanner(SensorClassifier classifier)
: classifier_(std::move(classifier))
{
}

std::vector<DiscoveredTopic> TopicScanner::scan(const rclcpp::Node::SharedPtr & node) const
{
  std::vector<DiscoveredTopic> topics;

  if (!node) {
    return topics;
  }

  const auto names_and_types = node->get_topic_names_and_types();
  for (const auto & [topic_name, message_types] : names_and_types) {
    for (const auto & message_type : message_types) {
      if (!classifier_.isSupportedMessageType(message_type)) {
        continue;
      }

      const auto topic_profile = classifier_.classifyTopic(topic_name, message_type);
      topics.push_back(DiscoveredTopic{
        topic_name,
        message_type,
        topic_profile.category,
        true,
        node->count_publishers(topic_name),
        topic_profile.group_key,
        topic_profile.group_label,
        topic_profile.topic_label
      });
    }
  }

  std::sort(
    topics.begin(),
    topics.end(),
    [](const DiscoveredTopic & left, const DiscoveredTopic & right) {
      return left.name < right.name;
    });

  return topics;
}

}  // namespace rviz_auto_sensor_panel
