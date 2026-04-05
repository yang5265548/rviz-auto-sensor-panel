#include "rviz_auto_sensor_panel/sensor_catalog.hpp"

#include <set>

namespace rviz_auto_sensor_panel
{

void SensorCatalog::update(const std::vector<DiscoveredTopic> & latest_topics)
{
  std::set<std::string> seen_topics;

  for (const auto & topic : latest_topics) {
    topics_by_name_[topic.name] = topic;
    topics_by_name_[topic.name].is_available = true;
    seen_topics.insert(topic.name);
  }

  for (auto & [topic_name, topic] : topics_by_name_) {
    if (seen_topics.find(topic_name) == seen_topics.end()) {
      topic.is_available = false;
    }
  }
}

std::vector<DiscoveredTopic> SensorCatalog::allTopics() const
{
  std::vector<DiscoveredTopic> topics;
  topics.reserve(topics_by_name_.size());

  for (const auto & [_, topic] : topics_by_name_) {
    topics.push_back(topic);
  }

  return topics;
}

std::map<SensorCategory, std::vector<DiscoveredTopic>> SensorCatalog::groupedTopics() const
{
  std::map<SensorCategory, std::vector<DiscoveredTopic>> groups;

  for (const auto & category : supportedCategories()) {
    groups[category] = {};
  }

  for (const auto & [_, topic] : topics_by_name_) {
    groups[topic.category].push_back(topic);
  }

  return groups;
}

}  // namespace rviz_auto_sensor_panel
