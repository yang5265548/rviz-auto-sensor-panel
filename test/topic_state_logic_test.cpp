#include "gtest/gtest.h"

#include "rviz_auto_sensor_panel/topic_state_logic.hpp"

namespace rviz_auto_sensor_panel
{

TEST(TopicStateLogicTest, autoEnableOnlyActivatesNewUnsuppressedTopics)
{
  std::set<std::string> enabled_topics{"/already/enabled"};
  std::set<std::string> suppressed_topics{"/demo/rear/scan"};
  const std::set<std::string> known_topics{"/already/enabled", "/demo/rear/scan"};

  applyAutoEnableForNewTopics(
    known_topics,
    {
      DiscoveredTopic{"/already/enabled", "sensor_msgs/msg/LaserScan", SensorCategory::Lidar, true},
      DiscoveredTopic{"/demo/rear/scan", "sensor_msgs/msg/LaserScan", SensorCategory::Lidar, true},
      DiscoveredTopic{"/demo/front/scan", "sensor_msgs/msg/LaserScan", SensorCategory::Lidar, true}
    },
    true,
    suppressed_topics,
    &enabled_topics);

  EXPECT_EQ(enabled_topics.size(), 2u);
  EXPECT_TRUE(enabled_topics.count("/already/enabled") > 0);
  EXPECT_TRUE(enabled_topics.count("/demo/front/scan") > 0);
  EXPECT_FALSE(enabled_topics.count("/demo/rear/scan") > 0);
}

TEST(TopicStateLogicTest, rememberTopicEnabledStateTracksSuppression)
{
  std::set<std::string> enabled_topics{"/demo/front/scan"};
  std::set<std::string> suppressed_topics;

  rememberTopicEnabledState(
    "/demo/front/scan",
    false,
    &enabled_topics,
    &suppressed_topics);

  EXPECT_FALSE(enabled_topics.count("/demo/front/scan") > 0);
  EXPECT_TRUE(suppressed_topics.count("/demo/front/scan") > 0);

  rememberTopicEnabledState(
    "/demo/front/scan",
    true,
    &enabled_topics,
    &suppressed_topics);

  EXPECT_TRUE(enabled_topics.count("/demo/front/scan") > 0);
  EXPECT_FALSE(suppressed_topics.count("/demo/front/scan") > 0);
}

TEST(TopicStateLogicTest, disableAllSuppressesEveryKnownTopic)
{
  std::set<std::string> enabled_topics;
  std::set<std::string> suppressed_topics;
  const std::vector<DiscoveredTopic> topics{
    DiscoveredTopic{"/demo/front/scan", "sensor_msgs/msg/LaserScan", SensorCategory::Lidar, true},
    DiscoveredTopic{"/demo/camera/left/image_raw", "sensor_msgs/msg/Image", SensorCategory::Camera, true}
  };

  setAllTopicStates(topics, false, &enabled_topics, &suppressed_topics);

  EXPECT_TRUE(enabled_topics.empty());
  EXPECT_EQ(suppressed_topics.size(), 2u);
}

}  // namespace rviz_auto_sensor_panel
