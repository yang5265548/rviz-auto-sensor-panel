# RViz Auto Sensor Panel Architecture

## 1. 文档目标 | Document Goal

本文档定义 `rviz-auto-sensor-panel` 的第一版系统架构、技术选型、模块职责与包结构。  
This document defines the first-version system architecture, technology choices, module responsibilities, and package structure for `rviz-auto-sensor-panel`.

它承接 MVP 基线文档，并为后续脚手架开发和功能实现提供统一设计基线。  
It follows the MVP baseline document and provides a shared design baseline for scaffolding and implementation.

## 2. 技术选型 | Technology Choices

### 2.1 主语言 | Primary Language

主开发语言定为 `C++`。  
The primary implementation language will be `C++`.

原因：  
Reasons:

- RViz 2 的插件体系天然以 C++ 为核心。  
  RViz 2's plugin system is naturally centered around C++.
- `Panel` 插件、display 管理、Qt Widgets 集成都更适合 C++。  
  `Panel` plugins, display management, and Qt Widgets integration fit C++ best.
- 与 `rclcpp`、`pluginlib`、`rviz_common`、`rviz_default_plugins` 的集成更直接。  
  Integration with `rclcpp`, `pluginlib`, `rviz_common`, and `rviz_default_plugins` is more direct.
- 这条路线能避免为了绕开 C++ 而引入额外复杂度。  
  This path avoids introducing extra complexity just to avoid C++.

### 2.2 辅助语言 | Secondary Language

辅助脚本和开发工具可按需使用 `Python`。  
`Python` may be used for helper scripts and development tooling when needed.

例如：  
Examples:

- 测试 topic 生成脚本  
  Test topic generator scripts
- 仿真数据准备工具  
  Simulation data preparation tools
- 开发辅助命令  
  Development helper commands

### 2.3 UI 技术 | UI Technology

界面层采用 `Qt Widgets`。  
The UI layer will use `Qt Widgets`.

原因：  
Reasons:

- RViz 2 本身就是基于 Qt Widgets。  
  RViz 2 itself is based on Qt Widgets.
- Panel 插件与 Widgets 集成自然。  
  Panel plugins integrate naturally with Widgets.
- 便于和 RViz 现有面板体验保持一致。  
  It helps keep the plugin aligned with existing RViz panel UX.

### 2.4 构建与打包 | Build and Packaging

- 构建系统：`ament_cmake`  
  Build system: `ament_cmake`
- C++ 标准：`C++17`  
  C++ standard: `C++17`
- ROS 2 客户端库：`rclcpp`  
  ROS 2 client library: `rclcpp`
- 插件机制：`pluginlib`  
  Plugin mechanism: `pluginlib`

## 3. 设计原则 | Design Principles

- 复用 RViz 能力，不重写渲染器。  
  Reuse RViz capabilities instead of rewriting the renderer.
- 先做自动化能力，再做复杂智能推断。  
  Prioritize automation before complex inference.
- 让集成层尽量薄，让业务逻辑保持清晰可测。  
  Keep the integration layer thin and the business logic clear and testable.
- 代码风格强调可读性，避免炫技式 C++。  
  Favor readability and avoid overly clever C++.
- 为未来增加更多传感器类型预留扩展点。  
  Leave extension points for more sensor types in the future.

## 4. 系统边界 | System Boundary

本项目是一个嵌入 RViz 的插件，而不是独立可视化软件。  
This project is an embedded RViz plugin, not a standalone visualization application.

它负责：  
It is responsible for:

- 发现 ROS graph 中受支持的 topics  
  Discovering supported topics in the ROS graph
- 对 topic 做基础分类  
  Performing basic topic classification
- 在自定义面板中展示结构化列表  
  Showing a structured list in a custom panel
- 代表用户创建和管理 RViz displays  
  Creating and managing RViz displays on behalf of the user

它不负责：  
It is not responsible for:

- 自定义底层图形渲染  
  Custom low-level rendering
- 替换 RViz 的主窗口或整体布局系统  
  Replacing RViz's main window or overall layout system
- 处理非 ROS 数据源  
  Handling non-ROS data sources

## 5. 高层架构 | High-Level Architecture

系统可分为四层：  
The system can be divided into four layers:

1. `ROS Graph Access Layer`  
   负责读取 topics 与消息类型。  
   Responsible for reading topics and message types.
2. `Domain Logic Layer`  
   负责分类、状态管理、显示映射。  
   Responsible for classification, state handling, and display mapping.
3. `RViz Integration Layer`  
   负责创建、查找、销毁和同步 RViz displays。  
   Responsible for creating, locating, destroying, and synchronizing RViz displays.
4. `UI Layer`  
   负责树形面板展示和用户交互。  
   Responsible for tree presentation and user interaction.

## 6. 核心模块 | Core Modules

### 6.1 `TopicScanner`

职责：  
Responsibilities:

- 读取当前 ROS 2 graph 中的 topics  
  Read topics from the current ROS 2 graph
- 获取每个 topic 的消息类型  
  Get the message type of each topic
- 过滤出受支持类型  
  Filter supported types
- 在 graph 变化后触发刷新  
  Trigger refresh after graph changes

输入：  
Inputs:

- ROS graph / node interface

输出：  
Outputs:

- `DiscoveredTopic` 列表  
  List of `DiscoveredTopic`

### 6.2 `SensorClassifier`

职责：  
Responsibilities:

- 根据消息类型将 topic 映射到传感器类别  
  Map topics to sensor categories based on message type
- 提供统一分类接口  
  Provide a unified classification interface
- 为未来增加更多传感器类型提供扩展点  
  Provide extension points for future sensor types

初始映射：  
Initial mapping:

- `sensor_msgs/msg/LaserScan` -> `Lidar`
- `sensor_msgs/msg/Image` -> `Camera`
- `sensor_msgs/msg/PointCloud2` -> `PointCloud`

### 6.3 `SensorCatalog`

职责：  
Responsibilities:

- 保存当前已发现并受支持的 topics  
  Store currently discovered and supported topics
- 维护 topic 的分类结果与状态  
  Maintain topic classification results and state
- 为 UI 提供稳定的数据视图  
  Provide a stable data view for the UI

说明：  
Notes:

这是一个面向面板展示的数据模型，不直接操作 RViz display。  
This is a data model for panel presentation and does not directly manipulate RViz displays.

### 6.4 `DisplayRegistry`

职责：  
Responsibilities:

- 跟踪插件创建的 RViz displays  
  Track RViz displays created by the plugin
- 避免重复创建同一 topic 对应的 display  
  Prevent duplicate display creation for the same topic
- 提供 display 查找、注册、移除能力  
  Provide display lookup, registration, and removal

说明：  
Notes:

这个模块只管理“插件自己创建的 display”，不接管用户手动创建的其他 display。  
This module manages only displays created by the plugin and does not take over unrelated user-created displays.

### 6.5 `AutoDisplayFactory`

职责：  
Responsibilities:

- 根据 topic 类型创建匹配的 RViz display  
  Create matching RViz displays based on topic type
- 设置必要的默认属性，例如 topic  
  Set required default properties such as the topic
- 屏蔽 RViz display 创建细节  
  Hide RViz display creation details

初始映射：  
Initial mapping:

- `LaserScan` -> RViz `LaserScan` display
- `Image` -> RViz `Image` display
- `PointCloud2` -> RViz `PointCloud2` display

### 6.6 `SensorTreeModel`

职责：  
Responsibilities:

- 将 catalog 数据组织为树结构  
  Organize catalog data into a tree structure
- 向 Qt 视图提供展示模型  
  Provide a presentation model to Qt views
- 维护分组、排序和显示标签  
  Maintain grouping, ordering, and display labels

### 6.7 `SensorTreePanel`

职责：  
Responsibilities:

- 提供 RViz Panel 插件入口  
  Provide the RViz Panel plugin entry point
- 展示树形 UI  
  Render the tree UI
- 处理启用、禁用、刷新等用户交互  
  Handle user interactions such as enable, disable, and refresh
- 协调 scanner、catalog、factory 和 registry  
  Coordinate the scanner, catalog, factory, and registry

## 7. 数据模型 | Data Model

建议在第一版中定义几个简单而明确的数据结构。  
The first version should define a few simple and explicit data structures.

### 7.1 `DiscoveredTopic`

建议字段：  
Suggested fields:

- `name`
- `message_type`
- `sensor_category`
- `is_available`

### 7.2 `ManagedDisplay`

建议字段：  
Suggested fields:

- `topic_name`
- `message_type`
- `display_type`
- `display_name`
- `is_enabled`

### 7.3 `SensorCategory`

建议先用枚举表示：  
Suggested as an enum for the first version:

- `Unknown`
- `Lidar`
- `Camera`
- `PointCloud`

## 8. 主要数据流 | Main Data Flow

### 8.1 启动流程 | Startup Flow

1. RViz 加载 `SensorTreePanel` 插件。  
   RViz loads the `SensorTreePanel` plugin.
2. Panel 初始化 ROS / RViz 依赖对象。  
   The panel initializes ROS and RViz dependencies.
3. `TopicScanner` 扫描当前 topics。  
   `TopicScanner` scans current topics.
4. `SensorClassifier` 对受支持 topics 分类。  
   `SensorClassifier` classifies supported topics.
5. `SensorCatalog` 更新内部数据。  
   `SensorCatalog` updates internal data.
6. `SensorTreeModel` 刷新树结构。  
   `SensorTreeModel` refreshes the tree structure.
7. UI 显示当前可用传感器项。  
   The UI shows currently available sensor items.

### 8.2 启用 Topic 流程 | Enable Topic Flow

1. 用户在面板中启用某个 topic。  
   The user enables a topic from the panel.
2. `DisplayRegistry` 检查是否已存在受管 display。  
   `DisplayRegistry` checks whether a managed display already exists.
3. 若不存在，`AutoDisplayFactory` 创建对应 RViz display。  
   If not, `AutoDisplayFactory` creates the matching RViz display.
4. 设置 display 的 topic 等属性。  
   The display's topic and related properties are configured.
5. `DisplayRegistry` 记录该 display。  
   `DisplayRegistry` records the display.
6. UI 状态更新为已启用。  
   The UI state is updated to enabled.

### 8.3 Topic 下线流程 | Topic Offline Flow

1. `TopicScanner` 发现某个受支持 topic 消失。  
   `TopicScanner` detects that a supported topic has disappeared.
2. `SensorCatalog` 将其标记为离线或不可用。  
   `SensorCatalog` marks it as offline or unavailable.
3. UI 更新显示状态。  
   The UI updates the visible state.
4. 对于已启用 display，MVP 阶段先保留 display，由用户自行关闭。  
   For enabled displays, the MVP keeps the display and lets the user disable it manually.

## 9. Graph 更新策略 | Graph Update Strategy

第一版建议采用“定时轮询优先，后续再优化”的方式。  
The first version should prefer a timer-based polling strategy and optimize later if needed.

原因：  
Reasons:

- 实现简单，便于尽快验证端到端功能。  
  It is simple to implement and helps validate end-to-end behavior quickly.
- 更容易调试动态 topic 上下线。  
  It is easier to debug dynamic topic appearance and disappearance.
- 可以先把产品体验跑通，再决定是否引入 graph event 优化。  
  It lets us validate the product experience before introducing graph-event optimizations.

第一版建议：  
Initial suggestion:

- 每 `1` 秒刷新一次 topic 列表  
  Refresh the topic list every `1` second

后续可选优化：  
Possible future optimizations:

- graph 事件与轮询混合  
  Hybrid graph events plus polling
- 在稳定状态下降低刷新频率  
  Lower refresh rate in stable states

## 10. Display 所有权策略 | Display Ownership Strategy

插件必须能够识别“哪些 display 是自己创建的”。  
The plugin must be able to identify which displays it created.

第一版建议策略：  
Suggested first-version strategy:

- 自动创建 display 时，为其生成统一命名规则  
  Generate a consistent naming rule when auto-creating displays
- 在 `DisplayRegistry` 中保存 topic 到 display 的映射  
  Store topic-to-display mappings in `DisplayRegistry`
- 只操作 registry 中登记过的对象  
  Only operate on objects registered in the registry

示例命名：  
Example naming:

- `[Auto] LaserScan: /front/scan`
- `[Auto] Image: /camera/left/image_raw`

## 11. 包结构建议 | Proposed Package Structure

第一阶段建议先使用单 package 结构，加快落地速度。  
The first implementation should start with a single-package structure to maximize delivery speed.

建议 package 名称：  
Suggested package name:

- `rviz_auto_sensor_panel`

建议目录：  
Suggested directories:

- `include/rviz_auto_sensor_panel/`  
  头文件目录  
  Header files
- `src/`  
  C++ 实现  
  C++ implementation
- `resource/`  
  插件描述文件、图标等资源  
  Plugin descriptions, icons, and related resources
- `test/`  
  单元测试和后续集成测试  
  Unit tests and future integration tests
- `docs/`  
  产品与架构文档  
  Product and architecture documents

## 12. 文件级初始拆分建议 | Initial File-Level Split

建议第一版从这些文件开始：  
Suggested initial files:

- `include/rviz_auto_sensor_panel/topic_scanner.hpp`
- `include/rviz_auto_sensor_panel/sensor_classifier.hpp`
- `include/rviz_auto_sensor_panel/sensor_catalog.hpp`
- `include/rviz_auto_sensor_panel/display_registry.hpp`
- `include/rviz_auto_sensor_panel/auto_display_factory.hpp`
- `include/rviz_auto_sensor_panel/sensor_tree_panel.hpp`
- `src/topic_scanner.cpp`
- `src/sensor_classifier.cpp`
- `src/sensor_catalog.cpp`
- `src/display_registry.cpp`
- `src/auto_display_factory.cpp`
- `src/sensor_tree_panel.cpp`

## 13. 可读性策略 | Readability Strategy

因为项目协作方并不以 C++ 为主语言，代码应刻意保持“易读 C++”风格。  
Because the collaborators are not primarily C++ developers, the code should intentionally stay in a "readable C++" style.

具体约束：  
Specific constraints:

- 尽量使用直接、清晰的类与方法命名  
  Prefer direct and clear class and method names
- 避免复杂模板技巧  
  Avoid complex template techniques
- 避免宏驱动设计  
  Avoid macro-heavy design
- 优先使用简单数据结构  
  Prefer simple data structures
- 关键逻辑前补充简短注释  
  Add brief comments before non-obvious logic

## 14. 下一阶段输出 | Next Phase Outputs

在这份架构文档确认后，下一阶段应产出：  
After this architecture document is accepted, the next phase should produce:

- ROS 2 package 脚手架  
  ROS 2 package scaffolding
- 插件描述文件  
  Plugin description files
- 最小可加载的 RViz Panel 插件  
  A minimally loadable RViz Panel plugin
- 初版 topic discovery 与树状 UI 骨架  
  Initial topic discovery and tree UI skeleton

## 15. 本阶段退出条件 | Exit Criteria for This Phase

当以下条件满足时，架构设计阶段可以结束：  
The architecture phase can end when:

- 技术选型被接受  
  The technology choices are accepted
- 高层模块拆分清晰  
  The high-level module split is clear
- 包结构可直接进入脚手架实现  
  The package structure is ready for scaffolding
- 关键设计风险已有初步处理策略  
  Key design risks have initial mitigation strategies
