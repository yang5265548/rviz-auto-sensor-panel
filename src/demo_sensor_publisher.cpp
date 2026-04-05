#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/image.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"
#include "sensor_msgs/msg/point_cloud2.hpp"
#include "sensor_msgs/msg/point_field.hpp"

namespace rviz_auto_sensor_panel
{

using namespace std::chrono_literals;

class DemoSensorPublisher : public rclcpp::Node
{
public:
  DemoSensorPublisher()
  : Node("demo_sensor_publisher"), angle_(0.0F)
  {
    front_scan_publisher_ =
      create_publisher<sensor_msgs::msg::LaserScan>("/demo/front/scan", 10);
    rear_scan_publisher_ =
      create_publisher<sensor_msgs::msg::LaserScan>("/demo/rear/scan", 10);
    left_image_publisher_ =
      create_publisher<sensor_msgs::msg::Image>("/demo/camera/left/image_raw", 10);
    right_image_publisher_ =
      create_publisher<sensor_msgs::msg::Image>("/demo/camera/right/image_raw", 10);
    point_cloud_publisher_ =
      create_publisher<sensor_msgs::msg::PointCloud2>("/demo/lidar/points", 10);

    timer_ = create_wall_timer(200ms, [this]() {
      publishFrontScan();
      publishRearScan();
      publishLeftImage();
      publishRightImage();
      publishPointCloud();
      angle_ += 0.15F;
    });
  }

private:
  sensor_msgs::msg::LaserScan buildScan(float phase_offset) const
  {
    sensor_msgs::msg::LaserScan scan;
    scan.header.stamp = now();
    scan.header.frame_id = "map";
    scan.angle_min = -1.57F;
    scan.angle_max = 1.57F;
    scan.angle_increment = 0.05F;
    scan.range_min = 0.1F;
    scan.range_max = 12.0F;

    const int sample_count = static_cast<int>(
      std::round((scan.angle_max - scan.angle_min) / scan.angle_increment)) + 1;
    scan.ranges.resize(sample_count);

    for (int index = 0; index < sample_count; ++index) {
      const float local_angle = scan.angle_min + static_cast<float>(index) * scan.angle_increment;
      scan.ranges[index] = 4.0F + 1.5F * std::sin(local_angle * 2.0F + angle_ + phase_offset);
    }

    return scan;
  }

  sensor_msgs::msg::Image buildImage(int width, int height, int color_offset) const
  {
    sensor_msgs::msg::Image image;
    image.header.stamp = now();
    image.header.frame_id = "map";
    image.height = static_cast<uint32_t>(height);
    image.width = static_cast<uint32_t>(width);
    image.encoding = "rgb8";
    image.is_bigendian = false;
    image.step = static_cast<sensor_msgs::msg::Image::_step_type>(width * 3);
    image.data.resize(static_cast<std::size_t>(height * width * 3));

    for (int y = 0; y < height; ++y) {
      for (int x = 0; x < width; ++x) {
        const std::size_t pixel_index = static_cast<std::size_t>((y * width + x) * 3);
        image.data[pixel_index] = static_cast<uint8_t>((x + color_offset) % 255);
        image.data[pixel_index + 1] = static_cast<uint8_t>((y * 2 + color_offset) % 255);
        image.data[pixel_index + 2] = static_cast<uint8_t>(
          (static_cast<int>(angle_ * 32.0F) + x + y + color_offset) % 255);
      }
    }

    return image;
  }

  sensor_msgs::msg::PointCloud2 buildPointCloud() const
  {
    sensor_msgs::msg::PointCloud2 cloud;
    cloud.header.stamp = now();
    cloud.header.frame_id = "map";
    cloud.height = 1;
    cloud.width = 64;
    cloud.is_bigendian = false;
    cloud.is_dense = true;
    cloud.point_step = 12;
    cloud.row_step = cloud.point_step * cloud.width;

    cloud.fields.resize(3);
    cloud.fields[0].name = "x";
    cloud.fields[0].offset = 0;
    cloud.fields[0].datatype = sensor_msgs::msg::PointField::FLOAT32;
    cloud.fields[0].count = 1;
    cloud.fields[1].name = "y";
    cloud.fields[1].offset = 4;
    cloud.fields[1].datatype = sensor_msgs::msg::PointField::FLOAT32;
    cloud.fields[1].count = 1;
    cloud.fields[2].name = "z";
    cloud.fields[2].offset = 8;
    cloud.fields[2].datatype = sensor_msgs::msg::PointField::FLOAT32;
    cloud.fields[2].count = 1;

    cloud.data.resize(cloud.row_step);

    for (uint32_t index = 0; index < cloud.width; ++index) {
      const float t = static_cast<float>(index) / static_cast<float>(cloud.width);
      const float x = 3.0F * std::cos(angle_ + t * 6.28F);
      const float y = 3.0F * std::sin(angle_ + t * 6.28F);
      const float z = std::sin(angle_ * 0.5F + t * 12.56F);

      const std::size_t offset = static_cast<std::size_t>(index * cloud.point_step);
      std::memcpy(&cloud.data[offset], &x, sizeof(float));
      std::memcpy(&cloud.data[offset + 4], &y, sizeof(float));
      std::memcpy(&cloud.data[offset + 8], &z, sizeof(float));
    }

    return cloud;
  }

  void publishFrontScan()
  {
    front_scan_publisher_->publish(buildScan(0.0F));
  }

  void publishRearScan()
  {
    rear_scan_publisher_->publish(buildScan(1.4F));
  }

  void publishLeftImage()
  {
    left_image_publisher_->publish(buildImage(160, 120, 20));
  }

  void publishRightImage()
  {
    right_image_publisher_->publish(buildImage(160, 120, 120));
  }

  void publishPointCloud()
  {
    point_cloud_publisher_->publish(buildPointCloud());
  }

  float angle_;
  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Publisher<sensor_msgs::msg::LaserScan>::SharedPtr front_scan_publisher_;
  rclcpp::Publisher<sensor_msgs::msg::LaserScan>::SharedPtr rear_scan_publisher_;
  rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr left_image_publisher_;
  rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr right_image_publisher_;
  rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr point_cloud_publisher_;
};

}  // namespace rviz_auto_sensor_panel

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<rviz_auto_sensor_panel::DemoSensorPublisher>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
