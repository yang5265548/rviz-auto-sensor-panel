# RViz Manual Validation

## 1. 文档目标 | Document Goal

本文档描述如何手动验证 `rviz_auto_sensor_panel` 当前脚手架是否能在 RViz 中加载并完成基础自动发现。  
This document describes how to manually validate that the current `rviz_auto_sensor_panel` scaffold can load in RViz and perform basic auto-discovery.

## 2. 验证目标 | Validation Goals

本轮手工验证应确认：  
This round of manual validation should confirm:

- 插件能被 RViz 识别并添加为 Panel  
  RViz can discover and add the plugin as a Panel
- 面板能扫描当前 ROS graph  
  The panel can scan the current ROS graph
- 支持的 topic 会按类别出现在树中  
  Supported topics appear in the tree by category
- 勾选 topic 后会自动创建对应 display  
  Checking a topic automatically creates the matching display
- 保存 RViz 配置后，面板能恢复勾选和展开状态  
  After saving RViz config, the panel restores checked and expanded states

## 3. 前置条件 | Preconditions

- 已安装 ROS 2 `jazzy`  
  ROS 2 `jazzy` is installed
- 本项目已能成功执行以下命令：  
  The project can successfully run:

```bash
python3 -m pip install --target ./vendor -r requirements-dev.txt
./scripts/configure_local.sh
./scripts/build_local.sh
./scripts/test_local.sh
```

- 当前 shell 已 source 你的 ROS 2 环境  
  Your current shell has sourced the ROS 2 environment

## 4. 启动建议 | Suggested Startup

如果你使用本地 `cmake` 构建结果，可以先确认插件库已经生成在 `build/` 下。  
If you are using the local `cmake` build output, first confirm the plugin library was generated under `build/`.

然后在能识别该 package 的环境里启动 `rviz2`。  
Then start `rviz2` in an environment that can discover the package.

## 5. 准备测试 Topics | Prepare Test Topics

建议至少准备这三类 topic：  
Prepare at least these three topic types:

- `sensor_msgs/msg/LaserScan`
- `sensor_msgs/msg/Image`
- `sensor_msgs/msg/PointCloud2`

如果当前机器人系统没有现成 topic，可以先用你自己的测试节点或仿真环境发布。  
If your robot system does not already provide these topics, use your own test nodes or simulation environment first.

## 6. 手工验证步骤 | Manual Validation Steps

### 6.1 加载 Panel | Load the Panel

1. 启动 `rviz2`  
   Start `rviz2`
2. 在顶部菜单中选择 `Panels -> Add New Panel`  
   In the top menu, choose `Panels -> Add New Panel`
3. 选择 `rviz_auto_sensor_panel / SensorTreePanel`  
   Select `rviz_auto_sensor_panel / SensorTreePanel`
4. 将 `Global Options -> Fixed Frame` 设为 `map`  
   Set `Global Options -> Fixed Frame` to `map`

预期结果：  
Expected result:

- 面板成功出现  
  The panel appears successfully
- 顶部状态栏显示 supported topics 统计信息  
  The top status label shows supported-topic statistics

### 6.2 自动发现 Topics | Verify Topic Discovery

1. 确保测试 topic 正在发布  
   Make sure test topics are being published
2. 等待自动刷新，或点击 `Refresh`  
   Wait for auto-refresh or click `Refresh`

预期结果：  
Expected result:

- 面板中出现 `Lidar`、`Camera`、`Point Cloud` 分类  
  The panel shows `Lidar`, `Camera`, and `Point Cloud` categories
- 对应 topic 出现在分类下  
  Matching topics appear under those categories
- 下线 topic 会显示 `Offline`  
  Offline topics are shown as `Offline`

### 6.3 自动创建 Displays | Verify Automatic Display Creation

1. 在树中勾选一个 `LaserScan` topic  
   Check a `LaserScan` topic in the tree
2. 在 RViz 左侧 Displays 区域查看是否新增自动 display  
   Check the RViz Displays area for a new auto-created display
3. 对 `Image` 与 `PointCloud2` 重复相同步骤  
   Repeat for `Image` and `PointCloud2`

预期结果：  
Expected result:

- 每个 topic 只创建一个自动 display  
  Each topic creates only one auto display
- display 名称以 `[Auto]` 开头  
  The display name starts with `[Auto]`
- 取消勾选后，该 display 被禁用而不是重复创建新实例  
  Unchecking disables the display instead of creating duplicate instances

### 6.4 验证状态恢复 | Verify State Restoration

1. 保持至少一个分类折叠、一个分类展开  
   Leave at least one category collapsed and one expanded
2. 勾选至少一个 topic  
   Check at least one topic
3. 保存 RViz 配置  
   Save the RViz configuration
4. 重新打开 RViz 并加载该配置  
   Re-open RViz and load that configuration

预期结果：  
Expected result:

- 勾选状态被恢复  
  Checked state is restored
- 分类展开状态被恢复  
  Category expansion state is restored
- 如果对应 topic 仍在线，自动 display 会再次启用  
  If the topic is still online, the auto display is re-enabled

## 7. 当前已知限制 | Current Known Limitations

- 目前只支持三种消息类型  
  Only three message types are currently supported
- 还没有复杂设备级聚合  
  There is no complex device-level grouping yet
- 还没有专门的自动化 UI 测试  
  There are no dedicated automated UI tests yet
- 如果某 topic 离线，已有 display 目前保留，由用户决定是否关闭  
  If a topic goes offline, the existing display is currently kept and the user decides whether to disable it

## 8. 建议记录方式 | Suggested Reporting Format

手工验证时，建议记录：  
During manual validation, record:

- 使用的 ROS 2 版本  
  ROS 2 version used
- 使用的测试 topics  
  Test topics used
- 成功项  
  Successful checks
- 失败项  
  Failed checks
- 截图或 screen recording  
  Screenshots or screen recordings
