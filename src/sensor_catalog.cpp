#include "rviz_auto_sensor_panel/sensor_catalog.hpp"

#include <algorithm>
#include <cctype>
#include <set>
#include <tuple>

namespace rviz_auto_sensor_panel
{

namespace
{

std::string toLowerCopy(const std::string & value)
{
  std::string lowered = value;
  std::transform(
    lowered.begin(), lowered.end(), lowered.begin(),
    [](unsigned char character) {return static_cast<char>(std::tolower(character));});
  return lowered;
}

int directionRank(const std::string & text)
{
  const auto lowered = toLowerCopy(text);
  if (lowered.find("front") != std::string::npos) {
    return 0;
  }
  if (lowered.find("left") != std::string::npos) {
    return 1;
  }
  if (lowered.find("center") != std::string::npos || lowered.find("centre") != std::string::npos) {
    return 2;
  }
  if (lowered.find("right") != std::string::npos) {
    return 3;
  }
  if (lowered.find("rear") != std::string::npos || lowered.find("back") != std::string::npos) {
    return 4;
  }
  if (lowered.find("top") != std::string::npos || lowered.find("upper") != std::string::npos) {
    return 5;
  }
  if (lowered.find("bottom") != std::string::npos || lowered.find("lower") != std::string::npos) {
    return 6;
  }
  return 7;
}

auto groupSortTuple(const TopicGroup & group)
{
  return std::make_tuple(directionRank(group.label), toLowerCopy(group.label), group.key);
}

}  // namespace

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
      topic.publisher_count = 0;
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
        return groupSortTuple(left) < groupSortTuple(right);
      });
  }

  return groups;
}

}  // namespace rviz_auto_sensor_panel
