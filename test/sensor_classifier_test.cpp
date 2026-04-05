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
  EXPECT_EQ(
    classifier.classifyMessageType("nav_msgs/msg/Odometry"),
    SensorCategory::Navigation);
  EXPECT_EQ(
    classifier.classifyMessageType("nav_msgs/msg/Path"),
    SensorCategory::Navigation);
  EXPECT_EQ(
    classifier.classifyMessageType("geometry_msgs/msg/PoseArray"),
    SensorCategory::Navigation);
  EXPECT_EQ(
    classifier.classifyMessageType("visualization_msgs/msg/Marker"),
    SensorCategory::Visualization);
  EXPECT_EQ(
    classifier.classifyMessageType("visualization_msgs/msg/MarkerArray"),
    SensorCategory::Visualization);
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
  EXPECT_EQ(
    classifier.lookupDisplayType("nav_msgs/msg/Odometry"),
    "rviz_default_plugins/Odometry");
  EXPECT_EQ(
    classifier.lookupDisplayType("nav_msgs/msg/Path"),
    "rviz_default_plugins/Path");
  EXPECT_EQ(
    classifier.lookupDisplayType("geometry_msgs/msg/PoseArray"),
    "rviz_default_plugins/PoseArray");
  EXPECT_EQ(
    classifier.lookupDisplayType("visualization_msgs/msg/Marker"),
    "rviz_default_plugins/Marker");
  EXPECT_EQ(
    classifier.lookupDisplayType("visualization_msgs/msg/MarkerArray"),
    "rviz_default_plugins/MarkerArray");
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

  const auto localization =
    classifier.classifyTopic("/demo/localization/odom", "nav_msgs/msg/Odometry");
  EXPECT_EQ(localization.category, SensorCategory::Navigation);
  EXPECT_EQ(localization.group_label, "Localization");

  const auto debug_markers =
    classifier.classifyTopic("/demo/debug/markers", "visualization_msgs/msg/MarkerArray");
  EXPECT_EQ(debug_markers.category, SensorCategory::Visualization);
  EXPECT_EQ(debug_markers.group_label, "Debug Visualization");
}

TEST(SensorClassifierTest, usesLeafTopicSegmentAsTopicLabel)
{
  SensorClassifier classifier;

  const auto image_topic =
    classifier.classifyTopic("/demo/camera/left/image_raw", "sensor_msgs/msg/Image");
  EXPECT_EQ(image_topic.topic_label, "image_raw");

  const auto lidar_topic =
    classifier.classifyTopic("/robot/front_camera/image_raw", "sensor_msgs/msg/Image");
  EXPECT_EQ(lidar_topic.group_label, "Robot Front Camera");
}

TEST(SensorClassifierTest, ignoresLeadingDemoContextInGroupLabels)
{
  SensorClassifier classifier;

  const auto demo_lidar =
    classifier.classifyTopic("/demo/front/scan", "sensor_msgs/msg/LaserScan");
  EXPECT_EQ(demo_lidar.group_label, "Front Lidar");
}

TEST(SensorClassifierTest, exposesLoadedGroupingRules)
{
  SensorClassifier classifier;

  EXPECT_TRUE(classifier.rules().ignored_leading_tokens.count("demo") > 0);
  EXPECT_EQ(classifier.rules().token_aliases.at("cam"), "camera");
}

}  // namespace rviz_auto_sensor_panel
