#include "gtest/gtest.h"

#include "rviz_auto_sensor_panel/sensor_classifier.hpp"

namespace rviz_auto_sensor_panel
{

TEST(SensorClassifierTest, classifiesSupportedTypes)
{
  SensorClassifier classifier;

  EXPECT_EQ(
    classifier.classifyMessageType("sensor_msgs/msg/LaserScan"),
    SensorCategory::Lidar);
  EXPECT_EQ(
    classifier.classifyMessageType("sensor_msgs/msg/Image"),
    SensorCategory::Camera);
  EXPECT_EQ(
    classifier.classifyMessageType("sensor_msgs/msg/PointCloud2"),
    SensorCategory::PointCloud);
}

TEST(SensorClassifierTest, rejectsUnsupportedTypes)
{
  SensorClassifier classifier;

  EXPECT_FALSE(classifier.isSupportedMessageType("std_msgs/msg/String"));
  EXPECT_EQ(classifier.classifyMessageType("std_msgs/msg/String"), SensorCategory::Unknown);
  EXPECT_TRUE(classifier.lookupDisplayType("std_msgs/msg/String").empty());
}

TEST(SensorClassifierTest, mapsToRvizDefaultPluginTypes)
{
  SensorClassifier classifier;

  EXPECT_EQ(
    classifier.lookupDisplayType("sensor_msgs/msg/LaserScan"),
    "rviz_default_plugins/LaserScan");
  EXPECT_EQ(
    classifier.lookupDisplayType("sensor_msgs/msg/Image"),
    "rviz_default_plugins/Image");
  EXPECT_EQ(
    classifier.lookupDisplayType("sensor_msgs/msg/PointCloud2"),
    "rviz_default_plugins/PointCloud2");
}

}  // namespace rviz_auto_sensor_panel
