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

- 自动发现 `LaserScan`、`Image`、`PointCloud2`  
  Automatically discovers `LaserScan`, `Image`, and `PointCloud2`
- 设备树分组与更友好的设备命名  
  Groups topics into a device tree with friendlier labels
- 勾选后自动创建对应 RViz display  
  Automatically creates matching RViz displays when checked
- `Enable All` / `Disable All` / `Auto-enable new topics`  
  `Enable All` / `Disable All` / `Auto-enable new topics`
- 记住手动关闭过的 topic，避免刷新后立即反弹  
  Remembers manually disabled topics to avoid immediate re-enabling on refresh
- 通过规则文件覆盖 topic 分组命名  
  Allows topic-grouping overrides through rule files
- Demo publisher、快速启动脚本与本地 smoke test  
  Includes a demo publisher, quick-launch scripts, and a local smoke test

## 下一步 | Next Steps

- 增加更多传感器类型支持  
  Add support for more sensor message types
- 引入更细粒度的设备模板与项目级规则  
  Introduce richer device templates and project-level rules
- 补更多 RViz 行为回归测试  
  Add more RViz behavior regression coverage
- 继续减少首次使用时的手工步骤  
  Continue reducing manual steps during first-time setup
