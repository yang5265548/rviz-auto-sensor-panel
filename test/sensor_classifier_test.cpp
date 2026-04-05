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

TEST(SensorClassifierTest, derivesFriendlyGroupLabelsFromCommonTopicConventions)
{
  SensorClassifier classifier;

  const auto front_lidar =
    classifier.classifyTopic("/robot/front/lidar/scan", "sensor_msgs/msg/LaserScan");
  EXPECT_EQ(front_lidar.category, SensorCategory::Lidar);
  EXPECT_EQ(front_lidar.group_label, "Robot Front Lidar");

  const auto left_camera =
    classifier.classifyTopic("/camera/left/image_raw", "sensor_msgs/msg/Image");
  EXPECT_EQ(left_camera.category, SensorCategory::Camera);
  EXPECT_EQ(left_camera.group_label, "Left Camera");

  const auto point_cloud =
    classifier.classifyTopic("/sensors/velodyne_points", "sensor_msgs/msg/PointCloud2");
  EXPECT_EQ(point_cloud.category, SensorCategory::PointCloud);
  EXPECT_EQ(point_cloud.group_label, "Sensors Point Cloud");
}

}  // namespace rviz_auto_sensor_panel
