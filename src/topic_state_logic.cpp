#include "rviz_auto_sensor_panel/topic_state_logic.hpp"

namespace rviz_auto_sensor_panel
{

void applyAutoEnableForNewTopics(
  const std::set<std::string> & known_topics,
  const std::vector<DiscoveredTopic> & latest_topics,
  bool auto_enable_new_topics,
  const std::set<std::string> & suppressed_topics,
  std::set<std::string> * enabled_topics)
{
  if (!auto_enable_new_topics || !enabled_topics) {
    return;
  }

  for (const auto & topic : latest_topics) {
    if (known_topics.count(topic.name) > 0) {
      continue;
    }

    if (suppressed_topics.count(topic.name) > 0) {
      continue;
    }

    enabled_topics->insert(topic.name);
  }
}

void rememberTopicEnabledState(
  const std::string & topic_name,
  bool enabled,
  std::set<std::string> * enabled_topics,
  std::set<std::string> * suppressed_topics)
{
  if (!enabled_topics || !suppressed_topics) {
    return;
  }

  if (enabled) {
    enabled_topics->insert(topic_name);
    suppressed_topics->erase(topic_name);
    return;
  }

  enabled_topics->erase(topic_name);
  suppressed_topics->insert(topic_name);
}

void setAllTopicStates(
  const std::vector<DiscoveredTopic> & topics,
  bool enabled,
  std::set<std::string> * enabled_topics,
  std::set<std::string> * suppressed_topics)
{
  if (!enabled_topics || !suppressed_topics) {
    return;
  }

  for (const auto & topic : topics) {
    rememberTopicEnabledState(topic.name, enabled, enabled_topics, suppressed_topics);
  }
}

}  // namespace rviz_auto_sensor_panel
