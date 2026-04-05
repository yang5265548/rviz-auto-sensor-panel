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
  ASSERT_EQ(groups.at(SensorCategory::Lidar)[0].topics.size(), 1u);
  ASSERT_EQ(groups.at(SensorCategory::Lidar)[1].topics.size(), 1u);
  ASSERT_EQ(groups.at(SensorCategory::Camera).size(), 1u);
  ASSERT_EQ(groups.at(SensorCategory::Camera)[0].topics.size(), 1u);
  ASSERT_EQ(groups.at(SensorCategory::PointCloud).size(), 0u);
}

TEST(SensorCatalogTest, mergesTopicsIntoStableDeviceGroups)
{
  SensorCatalog catalog;

  catalog.update({
    DiscoveredTopic{
      "/robot/front/lidar/scan",
      "sensor_msgs/msg/LaserScan",
      SensorCategory::Lidar,
      true,
      "lidar:robot_front",
      "Robot Front Lidar",
      "/robot/front/lidar/scan"
    },
    DiscoveredTopic{
      "/robot/front/lidar/scan_filtered",
      "sensor_msgs/msg/LaserScan",
      SensorCategory::Lidar,
      true,
      "lidar:robot_front",
      "Robot Front Lidar",
      "/robot/front/lidar/scan_filtered"
    }
  });

  const auto groups = catalog.groupedTopics();
  ASSERT_EQ(groups.at(SensorCategory::Lidar).size(), 1u);
  EXPECT_EQ(groups.at(SensorCategory::Lidar)[0].label, "Robot Front Lidar");
  EXPECT_EQ(groups.at(SensorCategory::Lidar)[0].topics.size(), 2u);
}

TEST(SensorCatalogTest, sortsDirectionalGroupsInOperatorFriendlyOrder)
{
  SensorCatalog catalog;

  catalog.update({
    DiscoveredTopic{
      "/robot/rear/scan",
      "sensor_msgs/msg/LaserScan",
      SensorCategory::Lidar,
      true,
      "lidar:rear",
      "Rear Lidar",
      "scan"
    },
    DiscoveredTopic{
      "/robot/front/scan",
      "sensor_msgs/msg/LaserScan",
      SensorCategory::Lidar,
      true,
      "lidar:front",
      "Front Lidar",
      "scan"
    },
    DiscoveredTopic{
      "/robot/left/scan",
      "sensor_msgs/msg/LaserScan",
      SensorCategory::Lidar,
      true,
      "lidar:left",
      "Left Lidar",
      "scan"
    }
  });

  const auto groups = catalog.groupedTopics();
  ASSERT_EQ(groups.at(SensorCategory::Lidar).size(), 3u);
  EXPECT_EQ(groups.at(SensorCategory::Lidar)[0].label, "Front Lidar");
  EXPECT_EQ(groups.at(SensorCategory::Lidar)[1].label, "Left Lidar");
  EXPECT_EQ(groups.at(SensorCategory::Lidar)[2].label, "Rear Lidar");
}

}  // namespace rviz_auto_sensor_panel
