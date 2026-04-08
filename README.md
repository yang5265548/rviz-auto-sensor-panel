# rviz-auto-sensor-panel

中文 | English

面向 ROS 2 的自动化 RViz 插件。  
An automation-oriented RViz plugin for ROS 2.

这个项目希望减少 RViz 中重复、机械的配置步骤：自动发现受支持的 ROS 2 可视化 topic，在树形面板中组织展示，并按需创建匹配的 RViz display。  
This project aims to reduce repetitive RViz setup by automatically discovering supported ROS 2 visualization topics, organizing them in a tree panel, and creating matching RViz displays on demand.

脚本现在默认会自动探测常见的 ROS 2 安装位置；如果你的机器使用自定义安装路径，可以设置 `RVIZ_AUTO_SENSOR_PANEL_ROS_SETUP=/absolute/path/to/setup.bash`。  
The scripts now auto-detect common ROS 2 install locations; if your machine uses a custom install path, set `RVIZ_AUTO_SENSOR_PANEL_ROS_SETUP=/absolute/path/to/setup.bash`.

## 愿景 | Vision

用户不应该手动一个个添加和配置 display，而应该可以：  
Instead of manually adding and configuring each display, users should be able to:

- 自动看到已发现的传感器  
  See discovered sensors automatically
- 在结构化面板中浏览它们  
  Browse them in a structured panel
- 以最少配置启用想看的传感器  
  Enable the sensor they want with minimal setup

## MVP | MVP

第一版聚焦以下能力：  
The first version focuses on:

- 自动发现受支持的 ROS 2 topics  
  Automatic discovery of supported ROS 2 topics
- 按消息类型分类  
  Classification by message type
- 树状 RViz 面板  
  A tree-based RViz panel
- 自动创建匹配的 RViz displays  
  Automatic creation of matching RViz displays

当前支持的消息类型：  
Currently supported message types:

- `sensor_msgs/msg/LaserScan`
- `sensor_msgs/msg/Image`
- `sensor_msgs/msg/PointCloud2`
- `nav_msgs/msg/OccupancyGrid`
- `nav_msgs/msg/Odometry`
- `nav_msgs/msg/Path`
- `geometry_msgs/msg/PoseStamped`
- `geometry_msgs/msg/PoseWithCovarianceStamped`
- `geometry_msgs/msg/PoseArray`
- `geometry_msgs/msg/PolygonStamped`
- `geometry_msgs/msg/PointStamped`
- `visualization_msgs/msg/Marker`
- `visualization_msgs/msg/MarkerArray`

## 项目状态 | Project Status

项目目前处于“可运行并可验证的 MVP”阶段。  
The project is currently at the "working and verifiable MVP" stage.

需求基线文档：  
Requirements baseline:

- [MVP Baseline / MVP 基线](./docs/0001-rviz-auto-sensor-panel-mvp.md)
- [Architecture / 架构设计](./docs/0002-rviz-auto-sensor-panel-architecture.md)
- [Development Setup / 开发环境](./docs/0003-development-setup.md)
- [RViz Manual Validation / 手工验证](./docs/0004-rviz-manual-validation.md)
- [Quick Start / 快速启动](./docs/0005-quick-start.md)
- [Topic Grouping Rules / Topic 分组规则](./docs/0006-topic-grouping-rules.md)
- [User Installation / 用户安装指南](./docs/0007-user-installation.md)
- [Release Readiness / 发布准备度](./docs/0008-release-readiness.md)
- [Maintainer Workflow / 维护者流程](./docs/0009-maintainer-workflow.md)
- [Main Merge Checklist / 合并清单](./docs/0010-main-merge-checklist.md)
- [Colleague Quick Guide / 同事快速指导](./docs/0011-colleague-quick-guide.md)

默认规则配置文件：  
Default rules configuration file:

- `config/topic_grouping_rules.ini`
- `config/example_topic_grouping_rules.ini`
- `config/default_demo.rviz`

仓库级文件：  
Repository-level files:

- `LICENSE`
- `CHANGELOG.md`
- `CONTRIBUTING.md`

## 计划目录 | Planned Structure

- `docs/`：产品、架构与设计文档  
  `docs/`: product, architecture, and design documents
- `src/`：实现代码包  
  `src/`: implementation packages
- `test/`：验证与集成支持  
  `test/`: validation and integration support

## 当前已具备 | What Works Today

- 自动发现 `LaserScan`、`Image`、`PointCloud2`、`OccupancyGrid`、`Odometry`、`Path`、`PoseStamped`、`PoseWithCovarianceStamped`、`PoseArray`、`PolygonStamped`、`PointStamped`、`Marker`、`MarkerArray`  
  Automatically discovers `LaserScan`, `Image`, `PointCloud2`, `OccupancyGrid`, `Odometry`, `Path`, `PoseStamped`, `PoseWithCovarianceStamped`, `PoseArray`, `PolygonStamped`, `PointStamped`, `Marker`, and `MarkerArray`
- 设备树分组与更友好的设备命名  
  Groups topics into a device tree with friendlier labels
- 覆盖传感器、地图、导航和调试可视化四个使用场景  
  Covers sensor, map, navigation, and debug-visualization use cases
- 勾选后自动创建对应 RViz display  
  Automatically creates matching RViz displays when checked
- 面板支持搜索过滤与离线 topic 可见性控制  
  The panel supports search filtering and offline-topic visibility control
- `Enable All` / `Disable All` / `Auto-enable new topics`  
  `Enable All` / `Disable All` / `Auto-enable new topics`
- 记住手动关闭过的 topic，避免刷新后立即反弹  
  Remembers manually disabled topics to avoid immediate re-enabling on refresh
- 通过规则文件覆盖 topic 分组命名  
  Allows topic-grouping overrides through rule files
- Demo publisher、快速启动脚本与本地 smoke test  
  Includes a demo publisher, quick-launch scripts, and a local smoke test

## 下一步 | Next Steps

- 引入更细粒度的设备模板与项目级规则  
  Introduce richer device templates and project-level rules
- 补更多 RViz 行为回归测试  
  Add more RViz behavior regression coverage
- 增加更丰富的面板信息与配置 UI  
  Add richer panel metadata and configuration UI
