# RViz Auto Sensor Panel MVP Baseline

## 1. 项目概述 | Project Summary

这个项目为 ROS 2 上的 RViz 增加一个面向自动化的插件。  
This project adds an automation-oriented plugin to RViz for ROS 2.

插件会自动发现运行中 ROS graph 里的传感器相关 topic，按消息类型进行分类，并以树状结构展示。用户不需要手动逐个添加和配置 display，而是可以直接在面板中展开传感器分组并启用所需项。  
The plugin will automatically discover sensor-related topics in the running ROS graph, classify them by message type, and present them in a structured tree view. Instead of manually adding and configuring each display one by one, the user can expand a sensor group and enable the desired items directly from the panel.

项目工作名：  
Working project name:

- `rviz_auto_sensor_panel`

## 2. 问题陈述 | Problem Statement

RViz 很强大，但在传感器较多的系统中，默认工作流会变得重复且低效。  
RViz is powerful, but the default workflow becomes repetitive in sensor-heavy systems.

常见痛点：  
Common pain points:

- 用户需要手动添加每一种 display。  
  Users need to manually add each display type.
- 用户需要手动设置每个 display 的 topic。  
  Users need to manually set the topic for each display.
- 多传感器机器人通常有大量相似 topic。  
  Multi-sensor robots often have many similar topics.
- 每次重复配置都很慢，而且容易出错。  
  Repeating setup across sessions is slow and error-prone.
- 新用户可能并不知道某个 ROS topic 应该对应 RViz 中哪种 display。  
  New users may not know which RViz display matches which ROS topic type.

这个插件的目标，是把多传感器可视化体验从“手动逐项配置”拉近到“接入即可检查”。  
This plugin is intended to reduce setup friction and make multi-sensor visualization feel closer to "plug and inspect" than "manually configure every display".

## 3. 产品目标 | Product Goal

创建一个 RViz 插件，自动检测常见传感器 topic，并以树形面板方式呈现，让用户能以尽可能少的手动配置来检查机器人可用传感器。  
Create an RViz plugin that automatically detects common sensor topics and exposes them in a tree panel, so users can inspect a robot's available sensors with minimal manual setup.

项目目标不是替代 RViz 的渲染能力，而是在 RViz 现有能力之上补齐自动发现与自动创建 display 的体验。  
The goal is not to replace RViz rendering, but to automate display discovery and creation on top of RViz's existing capabilities.

## 4. 目标用户 | Target Users

- ROS 2 开发者  
  ROS 2 developers
- 机器人专业学生  
  Robotics students
- 集成工程师  
  Integration engineers
- 测试与演示操作人员  
  Test and demo operators
- 处理多相机、多雷达、点云数据源的机器人团队  
  Teams working with robots that expose multiple cameras, lidars, and point cloud sources

## 5. 核心用户故事 | Primary User Stories

1. 作为开发者，我希望传感器 topic 能自动出现在 RViz 中，这样我就不需要手动逐个添加 display。  
   As a developer, I want sensor topics to appear automatically in RViz so I do not need to manually add displays one by one.
2. 作为需要处理多个同类传感器的用户，我希望它们被清晰分组，这样我能一眼看清当前有哪些数据源。  
   As a user working with multiple similar sensors, I want them grouped clearly so I can understand what is available at a glance.
3. 作为初学者，我希望系统能为 topic 选择正确的默认 display 类型，这样我不用先精通 RViz 才能开始。  
   As a beginner, I want the system to choose the correct default display type for a topic so I do not need deep RViz knowledge to get started.
4. 作为操作人员，我希望能在一个面板里快速启用或禁用已发现的传感器，用于测试和演示。  
   As an operator, I want to enable or disable discovered sensors quickly from one panel during testing or demos.

## 6. MVP 范围 | MVP Scope

MVP 包含以下能力。  
The MVP will include the following capabilities.

### 6.1 Topic 发现 | Topic Discovery

- 发现 ROS 2 graph 中的 topics。  
  Discover ROS 2 topics from the active graph.
- 过滤并保留受支持的传感器 topic 类型。  
  Filter and keep only supported sensor topic types.
- 当受支持的 topic 出现或消失时刷新面板。  
  Refresh the panel when supported topics appear or disappear.

### 6.2 支持的 Topic 类型 | Supported Topic Types

初始支持的消息类型：  
Initial supported message types:

- `sensor_msgs/msg/LaserScan`
- `sensor_msgs/msg/Image`
- `sensor_msgs/msg/PointCloud2`

选择这些类型的原因是它们常见、可视化意义强，而且已经被 RViz 很好支持。  
These types were selected because they are common, visually meaningful, and already well supported by RViz.

### 6.3 分类 | Classification

- 按消息类型对 topic 分类。  
  Classify topics by message type.
- 将每种支持类型映射到一个传感器类别：  
  Map each supported message type to a sensor category:
  - `LaserScan` -> Lidar
  - `Image` -> Camera
  - `PointCloud2` -> Point Cloud

### 6.4 树形界面 | Tree UI

- 提供一个 RViz panel 插件，带有树状视图。  
  Provide an RViz panel plugin with a tree view.
- 按传感器类别分组展示已发现 topics。  
  Group discovered topics by sensor category.
- 每个 topic 作为其类别下的子项显示。  
  Show each topic as a child item under its category.
- 显示基础状态，例如已启用或已禁用。  
  Show basic state such as enabled or disabled.

MVP 树结构示例：  
Example MVP tree:

- Lidar
- `/front/scan`
- `/rear/scan`
- Camera
- `/camera/left/image_raw`
- `/camera/right/image_raw`
- Point Cloud
- `/lidar/points`

### 6.5 自动创建 Display | Automatic Display Creation

- 当用户在面板中启用受支持 topic 时，自动创建匹配的 RViz display。  
  When a supported topic is enabled from the panel, automatically create the matching RViz display.
- 自动把该 display 绑定到发现到的 topic，并填充默认配置。  
  Bind the display to the discovered topic with a default configuration.
- 尽可能复用 RViz 已有 display 实现。  
  Reuse RViz's existing display implementations wherever possible.

初始类型映射：  
Initial type mapping:

- `LaserScan` topic -> RViz LaserScan display
- `Image` topic -> RViz Image display
- `PointCloud2` topic -> RViz PointCloud2 display

### 6.6 状态管理 | State Handling

- 跟踪已发现项当前是否启用。  
  Track whether a discovered item is currently enabled.
- 避免对同一个 topic 在启用状态下重复创建 display。  
  Avoid duplicate display creation for the same topic while enabled.
- 对从 graph 中消失的 topic 标记为不可用或离线。  
  Mark topics that disappear from the graph as unavailable or offline.

## 7. MVP 非目标 | Out of Scope for MVP

以下内容不属于第一版：  
The following will not be part of the first version.

- 复杂的设备级聚合，例如把 `image_raw` 和 `camera_info` 组合成一个 camera device  
  Complex device-level grouping such as combining `image_raw` and `camera_info` into one camera device
- 自动配对关联 topic  
  Automatic pairing of related topics
- 深度 topic 命名推断  
  Deep topic naming heuristics
- 项目级传感器模板  
  Per-project sensor templates
- 在自定义面板中提供复杂参数编辑  
  Advanced parameter editing inside the custom panel
- 自定义渲染实现  
  Custom rendering implementations
- 覆盖所有 RViz display 类型  
  Support for every RViz display type
- 自动布局或视口编排  
  Automatic layout or viewport arrangement
- 完整持久化全部用户自定义内容  
  Full persistence of all user customizations
- 非 ROS 数据源  
  Non-ROS data sources

## 8. 功能需求 | Functional Requirements

### 8.1 发现 | Discovery

- 插件应检查 ROS 2 graph 中可用的 topics。  
  The plugin shall inspect the ROS 2 graph for available topics.
- 插件应识别每个 topic 的消息类型。  
  The plugin shall identify the message type for each topic.
- 插件应仅在面板中纳入受支持的消息类型。  
  The plugin shall include only supported message types in the panel.

### 8.2 展示 | Presentation

- 插件应以按传感器类别分组的树形结构展示受支持 topics。  
  The plugin shall show supported topics in a tree grouped by sensor category.
- 插件应在 topic 被添加或移除时更新树。  
  The plugin shall update the tree when topics are added or removed.
- 插件应提供足够信息让用户快速识别 topic。  
  The plugin shall show enough information for the user to identify the topic quickly.

### 8.3 Display 管理 | Display Management

- 当用户启用某个 topic 时，插件应创建一个 RViz display。  
  The plugin shall create an RViz display when the user enables a topic.
- 插件应自动配置该 display 的 topic 属性。  
  The plugin shall configure the topic property automatically.
- 插件不应为同一个 topic 和同一种 display 类型重复创建活动 display。  
  The plugin shall not create duplicate active displays for the same topic and display type.
- 插件应允许用户从面板中禁用自动创建的 display。  
  The plugin shall allow the user to disable an auto-created display from the panel.

### 8.4 错误容忍 | Error Tolerance

- 如果 display 创建失败，插件应优雅失败。  
  The plugin shall fail gracefully if a display cannot be created.
- 插件应把不支持或异常的 topic 排除在管理列表之外。  
  The plugin shall keep unsupported or malformed topics out of the managed list.
- 当 topic 在运行时出现、消失或重启时，插件仍应保持可用。  
  The plugin shall remain usable when topics appear or disappear during runtime.

## 9. 非功能需求 | Non-Functional Requirements

- 在常见 topic 数量下，启动体验应足够即时。  
  Startup experience should feel immediate for common topic counts.
- 在 graph 更新期间，界面交互应保持响应。  
  UI interactions should remain responsive during graph updates.
- 实现应优先采用 RViz 原生扩展点，而不是修改 RViz 内部。  
  The plugin should prefer RViz-native extension points instead of patching RViz internals.
- 代码应具备足够模块化能力，以便后续扩展更多传感器类型。  
  The code should be modular enough to support more sensor types later.
- 实现应尽可能在不 fork RViz 的前提下具备可测试性。  
  The implementation should be testable without requiring a full custom RViz fork.

## 10. 技术方向建议 | Proposed Technical Direction

第一版实现应采用 RViz `Panel` 插件，加上一层小型管理逻辑，用于以编程方式创建标准 RViz displays。  
The first implementation should use an RViz `Panel` plugin plus a small management layer that creates standard RViz displays programmatically.

MVP 核心模块：  
Core modules for MVP:

- `TopicScanner`
- `SensorClassifier`
- `DisplayRegistry`
- `AutoDisplayFactory`
- `SensorTreePanel`

推荐这条路线的原因：  
This is the preferred direction because:

- 它能把自定义渲染工作降到最低。  
  It minimizes custom rendering work.
- 它可以复用稳定的 RViz display 插件。  
  It reuses stable RViz display plugins.
- 它把工程精力集中在真正的产品差异点上，也就是自动化能力。  
  It focuses engineering effort on the actual product differentiation: automation.

## 11. 成功标准 | Success Criteria

当以下条件全部满足时，MVP 可以视为成功：  
The MVP is successful if all of the following are true:

- 用户可以在启用插件的情况下启动 RViz。  
  A user can launch RViz with the plugin enabled.
- 受支持的 topics 会自动出现在面板里。  
  Supported topics appear automatically in the panel.
- 用户可以启用已发现 topic，而无需手动添加 display。  
  The user can enable a discovered topic without manually adding a display.
- 插件会为每种受支持消息类型创建正确的 RViz display。  
  The plugin creates the correct RViz display for each supported message type.
- 在至少包含 3 个 lidar 和 2 个 camera 的多传感器场景中，相比原生 RViz，手动配置工作量明显减少。  
  A multi-sensor setup with at least 3 lidars and 2 cameras can be inspected with substantially less manual setup than stock RViz.

## 12. 关键风险 | Key Risks

### 12.1 Topic 分组歧义 | Topic Grouping Ambiguity

不同项目中的 topic 命名差异很大，如果过早引入设备级推断，可靠性可能不足。  
Topic names vary across projects, so meaningful device grouping may be unreliable if introduced too early.

MVP 应对策略：  
MVP response:

- 仅按消息类型分组。  
  Group only by message type.
- 设备推断延后到后续里程碑。  
  Delay device inference to a later milestone.

### 12.2 RViz 集成复杂度 | RViz Integration Complexity

程序化创建 display 可能会涉及 RViz API 中的一些内部假设。  
Programmatic display creation may involve internal assumptions in RViz APIs.

MVP 应对策略：  
MVP response:

- 尽量依赖文档化的插件扩展点。  
  Rely on documented plugin extension points.
- 第一版保持范围收敛，只使用标准 display 类。  
  Keep the first implementation narrow and use standard display classes only.

### 12.3 动态 Graph 变化 | Dynamic Graph Changes

当界面打开时，topics 可能出现、消失或重启。  
Topics may appear, disappear, or restart while the UI is open.

MVP 应对策略：  
MVP response:

- 设计为可重复刷新。  
  Design for repeated refresh.
- 以稳定的 topic 身份和 display 类型跟踪受管对象。  
  Track managed displays by stable topic identity and display type.

### 12.4 默认配置质量 | Default Configuration Quality

如果默认参数质量不佳，自动创建 display 反而会显得嘈杂或困惑。  
Poor defaults could make automatic creation feel noisy or confusing.

MVP 应对策略：  
MVP response:

- 默认设置保持最小化。  
  Keep default settings minimal.
- 先暴露简单的启用和禁用控制。  
  Expose simple enable and disable control first.

## 13. 下一阶段待决事项 | Open Decisions for the Next Phase

这些问题不阻塞 MVP 基线，但需要在架构设计阶段解决：  
These questions do not block the MVP baseline, but they must be resolved during architecture design:

- 插件应如何发现 graph 变化：轮询、graph 事件，还是混合方案？  
  How should the plugin discover graph changes: polling, graph events, or a hybrid approach?
- 插件应如何只识别并管理自己创建的 displays？  
  How should the plugin identify and own only the displays it created?
- 当 topic 离线时，已禁用项是否应作为占位继续显示？  
  Should disabled topics remain visible as placeholders when they go offline?
- V1 需要持久化哪些元数据？  
  What metadata should be persisted between sessions in V1?
- 树中的命名和排序应如何规范化？  
  How should naming and ordering be normalized in the tree?

## 14. 第一阶段退出条件 | Exit Criteria for Phase 1

当以下条件成立时，第一阶段完成：  
Phase 1 is complete when:

- 团队同意产品目标。  
  The team agrees with the product goal.
- MVP 范围稳定。  
  The MVP scope is stable.
- 插件方案被接受。  
  The plugin approach is accepted.
- 下一阶段可以进入架构和 package 结构设计。  
  The next phase can move into architecture and package structure design.
