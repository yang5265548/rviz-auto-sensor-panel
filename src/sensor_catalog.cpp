#include "rviz_auto_sensor_panel/sensor_catalog.hpp"

#include <algorithm>
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

std::map<SensorCategory, std::vector<TopicGroup>> SensorCatalog::groupedTopics() const
{
  std::map<SensorCategory, std::vector<TopicGroup>> groups;
  std::map<SensorCategory, std::map<std::string, TopicGroup>> groups_by_key;

  for (const auto & category : supportedCategories()) {
    groups[category] = {};
  }

  for (const auto & [_, topic] : topics_by_name_) {
    auto & grouped_by_key = groups_by_key[topic.category];
    const auto effective_group_key = topic.group_key.empty() ? topic.name : topic.group_key;
    auto & group = grouped_by_key[effective_group_key];
    group.key = effective_group_key;
    group.category = topic.category;
    group.label = topic.group_label.empty() ? toString(topic.category) : topic.group_label;
    group.topics.push_back(topic);
  }

  for (auto & [category, grouped_by_key] : groups_by_key) {
    auto & category_groups = groups[category];
    for (auto & [_, group] : grouped_by_key) {
      std::sort(
        group.topics.begin(),
        group.topics.end(),
        [](const DiscoveredTopic & left, const DiscoveredTopic & right) {
          return left.name < right.name;
        });
      category_groups.push_back(group);
    }

    std::sort(
      category_groups.begin(),
      category_groups.end(),
      [](const TopicGroup & left, const TopicGroup & right) {
        return left.label < right.label;
      });
  }

  return groups;
}

}  // namespace rviz_auto_sensor_panel
