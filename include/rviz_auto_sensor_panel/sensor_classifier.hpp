#ifndef RVIZ_AUTO_SENSOR_PANEL__SENSOR_CLASSIFIER_HPP_
#define RVIZ_AUTO_SENSOR_PANEL__SENSOR_CLASSIFIER_HPP_

#include <map>
#include <set>
#include <string>
#include <vector>

#include "rviz_auto_sensor_panel/types.hpp"

namespace rviz_auto_sensor_panel
{

struct TopicProfile
{
  SensorCategory category{SensorCategory::Unknown};
  std::string group_key;
  std::string group_label;
  std::string topic_label;
};

struct GroupingRules
{
  std::set<std::string> ignored_leading_tokens;
  std::map<std::string, std::string> token_aliases;
  std::vector<std::string> direction_priority;
};

class SensorClassifier
{
public:
  SensorClassifier();

  SensorCategory classifyMessageType(const std::string & message_type) const;
  TopicProfile classifyTopic(const std::string & topic_name, const std::string & message_type) const;
  bool isSupportedMessageType(const std::string & message_type) const;
  std::string lookupDisplayType(const std::string & message_type) const;
  const GroupingRules & rules() const;

private:
  GroupingRules rules_;
};

}  // namespace rviz_auto_sensor_panel

#endif  // RVIZ_AUTO_SENSOR_PANEL__SENSOR_CLASSIFIER_HPP_
