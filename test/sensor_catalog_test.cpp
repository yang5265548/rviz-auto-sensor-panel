#include "gtest/gtest.h"

#include "rviz_auto_sensor_panel/sensor_catalog.hpp"

namespace rviz_auto_sensor_panel
{

TEST(SensorCatalogTest, marksMissingTopicsAsOfflineInsteadOfDeletingThem)
{
  SensorCatalog catalog;

  catalog.update({
    DiscoveredTopic{"/front/scan", "sensor_msgs/msg/LaserScan", SensorCategory::Lidar, true},
    DiscoveredTopic{"/camera/left/image_raw", "sensor_msgs/msg/Image", SensorCategory::Camera, true}
  });

  catalog.update({
    DiscoveredTopic{"/front/scan", "sensor_msgs/msg/LaserScan", SensorCategory::Lidar, true}
  });

  const auto topics = catalog.allTopics();
  ASSERT_EQ(topics.size(), 2u);

  bool found_offline_camera = false;
  for (const auto & topic : topics) {
    if (topic.name == "/camera/left/image_raw") {
      found_offline_camera = true;
      EXPECT_FALSE(topic.is_available);
      EXPECT_EQ(topic.category, SensorCategory::Camera);
    }
  }

  EXPECT_TRUE(found_offline_camera);
}

TEST(SensorCatalogTest, groupsTopicsByCategory)
{
  SensorCatalog catalog;

  catalog.update({
    DiscoveredTopic{"/front/scan", "sensor_msgs/msg/LaserScan", SensorCategory::Lidar, true},
    DiscoveredTopic{"/rear/scan", "sensor_msgs/msg/LaserScan", SensorCategory::Lidar, true},
    DiscoveredTopic{"/camera/left/image_raw", "sensor_msgs/msg/Image", SensorCategory::Camera, true}
  });

  const auto groups = catalog.groupedTopics();

  ASSERT_EQ(groups.at(SensorCategory::Lidar).size(), 2u);
  ASSERT_EQ(groups.at(SensorCategory::Camera).size(), 1u);
  ASSERT_EQ(groups.at(SensorCategory::PointCloud).size(), 0u);
}

}  // namespace rviz_auto_sensor_panel
