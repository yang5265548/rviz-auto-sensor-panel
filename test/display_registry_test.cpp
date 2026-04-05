#include "gtest/gtest.h"

#include "rviz_auto_sensor_panel/display_registry.hpp"

namespace rviz_auto_sensor_panel
{

TEST(DisplayRegistryTest, storesManagedDisplayMetadataWithoutDisplayPointer)
{
  DisplayRegistry registry;

  registry.registerDisplay(
    ManagedDisplay{
      "/front/scan",
      "sensor_msgs/msg/LaserScan",
      "rviz_default_plugins/LaserScan",
      "[Auto] LaserScan: /front/scan",
      true
    },
    nullptr);

  EXPECT_TRUE(registry.hasDisplay("/front/scan"));
  EXPECT_TRUE(registry.isEnabled("/front/scan"));
  EXPECT_EQ(registry.getDisplay("/front/scan"), nullptr);
}

TEST(DisplayRegistryTest, tracksEnabledState)
{
  DisplayRegistry registry;

  registry.registerDisplay(
    ManagedDisplay{
      "/camera/left/image_raw",
      "sensor_msgs/msg/Image",
      "rviz_default_plugins/Image",
      "[Auto] Image: /camera/left/image_raw",
      true
    },
    nullptr);

  registry.setEnabled("/camera/left/image_raw", false);

  EXPECT_FALSE(registry.isEnabled("/camera/left/image_raw"));
}

}  // namespace rviz_auto_sensor_panel
