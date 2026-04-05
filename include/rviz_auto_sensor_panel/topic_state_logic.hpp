#ifndef RVIZ_AUTO_SENSOR_PANEL__TOPIC_STATE_LOGIC_HPP_
#define RVIZ_AUTO_SENSOR_PANEL__TOPIC_STATE_LOGIC_HPP_

#include <set>
#include <string>
#include <vector>

#include "rviz_auto_sensor_panel/types.hpp"

namespace rviz_auto_sensor_panel
{

void applyAutoEnableForNewTopics(
  const std::set<std::string> & known_topics,
  const std::vector<DiscoveredTopic> & latest_topics,
  bool auto_enable_new_topics,
  const std::set<std::string> & suppressed_topics,
  std::set<std::string> * enabled_topics);

void rememberTopicEnabledState(
  const std::string & topic_name,
  bool enabled,
  std::set<std::string> * enabled_topics,
  std::set<std::string> * suppressed_topics);

void setAllTopicStates(
  const std::vector<DiscoveredTopic> & topics,
  bool enabled,
  std::set<std::string> * enabled_topics,
  std::set<std::string> * suppressed_topics);

}  // namespace rviz_auto_sensor_panel

#endif  // RVIZ_AUTO_SENSOR_PANEL__TOPIC_STATE_LOGIC_HPP_
