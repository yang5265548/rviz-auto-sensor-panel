# Topic Grouping Rules

## 1. 文档目标 | Document Goal

本文档定义插件当前采用的 topic 分组策略，以及它如何从“仅按消息类型分类”演进为“兼顾主流 ROS 约定的设备分组”。  
This document defines the plugin's current topic grouping strategy and how it evolves from "message-type-only classification" toward "device-oriented grouping informed by common ROS conventions."

## 2. 设计原则 | Design Principles

分组规则分三层，不依赖单一项目的私有命名风格：  
The grouping rules use three layers and do not depend on one project's private naming style:

- 官方稳定层：先以 ROS 2 消息类型作为唯一可靠入口  
  Official stable layer: use ROS 2 message types as the first reliable signal
- 生态约定层：吸收 ROS 社区常见 topic 命名习惯  
  Ecosystem convention layer: absorb common topic naming habits from the ROS ecosystem
- 用户扩展层：后续支持项目自定义命名规则  
  User extension layer: support project-specific naming rules later

## 3. 当前支持的稳定信号 | Currently Supported Stable Signals

当前版本首先根据消息类型识别传感器类别：  
The current version first identifies sensor categories from message types:

- `sensor_msgs/msg/LaserScan` -> `Lidar`
- `sensor_msgs/msg/Image` -> `Camera`
- `sensor_msgs/msg/PointCloud2` -> `Point Cloud`
- `nav_msgs/msg/OccupancyGrid` -> `Map`
- `nav_msgs/msg/Odometry` / `nav_msgs/msg/Path` -> `Navigation`
- `geometry_msgs/msg/PoseStamped` / `geometry_msgs/msg/PoseWithCovarianceStamped` -> `Navigation`
- `geometry_msgs/msg/PoseArray` / `geometry_msgs/msg/PolygonStamped` / `geometry_msgs/msg/PointStamped` -> `Navigation`
- `visualization_msgs/msg/Marker` / `visualization_msgs/msg/MarkerArray` -> `Visualization`

这是插件通用性的基础，因为 ROS 2 官方定义了消息类型，但没有统一规定所有团队必须使用完全相同的设备 topic 名称。  
This is the foundation of portability because ROS 2 standardizes message types, but it does not require all teams to use identical device topic names.

## 4. 当前吸收的命名约定 | Naming Conventions Used Today

在消息类型确定后，插件会从 topic 名中提取更友好的设备组名称。  
After the message type is known, the plugin extracts a friendlier device-group name from the topic itself.

### 4.1 相机 | Camera

插件会识别这些常见相机后缀并把它们视为“流类型”而不是设备名的一部分：  
The plugin treats these common camera suffixes as stream variants rather than part of the device identity:

- `image_raw`
- `image_rect`
- `image_rect_color`
- `camera_info`
- `compressed`

例如：  
For example:

- `/camera/left/image_raw` -> `Left Camera`
- `/robot/front_camera/image_rect` -> `Robot Front Camera`

### 4.2 激光扫描 | Laser Scan

插件会把这些常见雷达结尾视为数据流描述，而不是设备名本体：  
The plugin treats these common lidar endings as data-stream descriptors rather than the core device name:

- `scan`
- `laserscan`
- `raw`
- `filtered`

例如：  
For example:

- `/front/lidar/scan` -> `Front Lidar`
- `/robot/rear/laser/scan` -> `Robot Rear Lidar`

### 4.3 点云 | Point Cloud

插件会把这些常见点云结尾视为数据流描述：  
The plugin treats these common point-cloud endings as stream descriptors:

- `points`
- `pointcloud`
- `point_cloud`
- `velodyne_points`

例如：  
For example:

- `/lidar/points` -> `Lidar Point Cloud`
- `/sensors/velodyne_points` -> `Sensors Point Cloud`

## 5. 当前的限制 | Current Limits

当前规则还是启发式的，不应误解为“ROS 官方统一标准”。  
The current rules are still heuristic and should not be mistaken for an official ROS-wide naming standard.

已知限制包括：  
Known limits include:

- 不同公司可能把相同设备命名成完全不同的 topic  
  Different companies may name the same device with completely different topics
- 单个设备可能同时发布多个支持 topic，目前只做基础聚合  
  One device may publish multiple supported topics, and only basic grouping is done today
- 部分厂商名、项目缩写、安装位姿词汇仍需要更多样本支持  
  Some vendor names, project abbreviations, and mounting-position terms still need more examples

## 6. 下一步演进 | Next Evolution

后续版本会继续增强通用性：  
Future versions will keep improving portability:

- 增加更多 ROS 常见命名模式  
  Add more common ROS naming patterns
- 支持用户自定义规则文件  
  Support user-defined rule files
- 把设备组和 frame、QoS、附属 topic 信息关联起来  
  Associate device groups with frame, QoS, and auxiliary topic information

## 7. 配置文件 | Configuration File

当前版本已经支持用规则文件覆盖默认分组行为。  
The current version already supports overriding default grouping behavior with a rules file.

默认安装路径：  
Default installed path:

- `share/rviz_auto_sensor_panel/config/topic_grouping_rules.ini`

也可以通过环境变量覆盖：  
You can also override it with an environment variable:

- `RVIZ_AUTO_SENSOR_PANEL_RULES_FILE=/absolute/path/to/topic_grouping_rules.ini`

当前支持的配置项：  
Currently supported configuration keys:

- `[grouping] ignored_leading_tokens`
- `[grouping] direction_priority`
- `[aliases]`

例如：  
For example:

```ini
[grouping]
ignored_leading_tokens=demo,simulation
direction_priority=front,left,right,rear

[aliases]
cam=camera
pc=pointcloud
```

仓库里还提供了一份可直接试跑的示例覆盖文件：  
The repository also includes an example override file you can try immediately:

- `config/example_topic_grouping_rules.ini`

## 8. 结论 | Conclusion

当前版本的分组不是“只针对你的电脑写死”，但也还没有达到“零配置覆盖所有 ROS 项目”的程度。  
The current grouping is not hard-coded only for your machine, but it also has not yet reached "zero-config coverage for every ROS project."

它现在处于一个比较合理的中间状态：  
It currently sits at a reasonable middle point:

- 用官方稳定信号保证基础正确性  
  Use official stable signals for baseline correctness
- 用主流命名约定提升可读性和自动化体验  
  Use mainstream naming conventions to improve readability and automation
- 为后续配置化扩展预留结构  
  Leave room for future configurable extensions
