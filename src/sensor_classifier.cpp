#include "rviz_auto_sensor_panel/sensor_classifier.hpp"

namespace rviz_auto_sensor_panel
{

namespace
{

constexpr char kLaserScanMessageType[] = "sensor_msgs/msg/LaserScan";
constexpr char kImageMessageType[] = "sensor_msgs/msg/Image";
constexpr char kPointCloud2MessageType[] = "sensor_msgs/msg/PointCloud2";

constexpr char kLaserScanDisplayType[] = "rviz_default_plugins/LaserScan";
constexpr char kImageDisplayType[] = "rviz_default_plugins/Image";
constexpr char kPointCloud2DisplayType[] = "rviz_default_plugins/PointCloud2";

}  // namespace

SensorCategory SensorClassifier::classifyMessageType(const std::string & message_type) const
{
  if (message_type == kLaserScanMessageType) {
    return SensorCategory::Lidar;
  }

  if (message_type == kImageMessageType) {
    return SensorCategory::Camera;
  }

  if (message_type == kPointCloud2MessageType) {
    return SensorCategory::PointCloud;
  }

  return SensorCategory::Unknown;
}

bool SensorClassifier::isSupportedMessageType(const std::string & message_type) const
{
  return classifyMessageType(message_type) != SensorCategory::Unknown;
}

std::string SensorClassifier::lookupDisplayType(const std::string & message_type) const
{
  if (message_type == kLaserScanMessageType) {
    return kLaserScanDisplayType;
  }

  if (message_type == kImageMessageType) {
    return kImageDisplayType;
  }

  if (message_type == kPointCloud2MessageType) {
    return kPointCloud2DisplayType;
  }

  return "";
}

std::string toString(SensorCategory category)
{
  switch (category) {
    case SensorCategory::Lidar:
      return "Lidar";
    case SensorCategory::Camera:
      return "Camera";
    case SensorCategory::PointCloud:
      return "Point Cloud";
    case SensorCategory::Unknown:
    default:
      return "Unknown";
  }
}

std::vector<SensorCategory> supportedCategories()
{
  return {
    SensorCategory::Lidar,
    SensorCategory::Camera,
    SensorCategory::PointCloud
  };
}

}  // namespace rviz_auto_sensor_panel
