# rviz-auto-sensor-panel

中文 | English

面向 ROS 2 的自动化 RViz 插件。  
An automation-oriented RViz plugin for ROS 2.

这个项目希望减少 RViz 中重复、机械的配置步骤：自动发现受支持的传感器 topic，在树形面板中组织展示，并按需创建匹配的 RViz display。  
This project aims to reduce repetitive RViz setup by automatically discovering supported sensor topics, organizing them in a tree panel, and creating matching RViz displays on demand.

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

初始支持的消息类型：  
Initial supported message types:

- `sensor_msgs/msg/LaserScan`
- `sensor_msgs/msg/Image`
- `sensor_msgs/msg/PointCloud2`

## 项目状态 | Project Status

项目目前处于“可运行 MVP 原型”阶段。  
The project is currently at the "working MVP prototype" stage.

需求基线文档：  
Requirements baseline:

- [MVP Baseline / MVP 基线](./docs/0001-rviz-auto-sensor-panel-mvp.md)
- [Architecture / 架构设计](./docs/0002-rviz-auto-sensor-panel-architecture.md)
- [Development Setup / 开发环境](./docs/0003-development-setup.md)
- [RViz Manual Validation / 手工验证](./docs/0004-rviz-manual-validation.md)
- [Quick Start / 快速启动](./docs/0005-quick-start.md)
- [Topic Grouping Rules / Topic 分组规则](./docs/0006-topic-grouping-rules.md)

## 计划目录 | Planned Structure

- `docs/`：产品、架构与设计文档  
  `docs/`: product, architecture, and design documents
- `src/`：实现代码包  
  `src/`: implementation packages
- `test/`：验证与集成支持  
  `test/`: validation and integration support

## 下一步 | Next Steps

- 完成插件架构设计  
  Finalize plugin architecture
- 扩展 topic discovery、状态同步和 display 管理逻辑  
  Extend topic discovery, state synchronization, and display management logic
- 增加更多测试与 RViz 手工验证步骤  
  Add more tests and an RViz manual verification flow
- 实现更完整的自动化交互体验  
  Implement a more complete automation-oriented interaction flow
