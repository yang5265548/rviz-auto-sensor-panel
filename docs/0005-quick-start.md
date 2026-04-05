# Quick Start

## 1. 文档目标 | Document Goal

本文档给出当前项目最直接的本地启动方式，目标是尽快把插件跑起来并看到自动发现效果。  
This document provides the most direct local startup path for the current project, with the goal of getting the plugin running and showing auto-discovery as quickly as possible.

## 2. 一次性准备 | One-Time Setup

在项目根目录执行：  
Run these from the project root:

```bash
./scripts/rebuild_install_local.sh
```

如果你只想最快打开“带 demo 数据的 RViz”，也可以直接用一键脚本：  
If you want the fastest way to open RViz with demo data already running, you can use the one-command script:

```bash
./scripts/run_demo_stack.sh
```

这个命令会使用项目自带的 RViz 预设，因此默认会直接带出 `SensorTreePanel`，并把 `Fixed Frame` 设成 `map`。  
This command uses the project RViz preset, so it should open with `SensorTreePanel` already loaded and `Fixed Frame` already set to `map`.

## 3. 启动演示数据 | Start Demo Topics

打开一个终端，source 你的 ROS 2 环境后执行：  
Open a terminal, source your ROS 2 environment, then run:

```bash
./scripts/run_demo_publisher.sh
```

这个进程会持续发布以下演示 topics：  
This process continuously publishes these demo topics:

- `/demo/front/scan`
- `/demo/rear/scan`
- `/demo/camera/left/image_raw`
- `/demo/camera/right/image_raw`
- `/demo/lidar/points`
- `/demo/localization/odom`
- `/demo/localization/pose_array`
- `/demo/navigation/path`
- `/demo/debug/markers`

如果你想先确认数据已经起来，可以在第三个终端执行：  
If you want to confirm the data is up first, run this in a third terminal:

```bash
./scripts/check_demo_topics.sh
```

## 4. 启动 RViz | Start RViz

打开第二个终端，source ROS 2 环境，并让 RViz 能找到本地安装结果。  
Open a second terminal, source ROS 2, and make sure RViz can discover the local install.

说明：当前仓库使用的是 package 级安装结果，因此脚本会加载  
本地 `install/` 前缀到 `AMENT_PREFIX_PATH` 与 `CMAKE_PREFIX_PATH`。  
Note: the startup script injects the local `install/` prefix into  
`AMENT_PREFIX_PATH` and `CMAKE_PREFIX_PATH`.

这样可以让 RViz 发现本地插件，同时避免破坏系统里默认 RViz plugins 的动态库加载。  
This allows RViz to discover the local plugin while avoiding interference with the dynamic library loading of the default RViz plugins.

建议顺序：  
Suggested order:

```bash
./scripts/run_rviz_local.sh
```

如果你想直接用项目预设启动 RViz，而不是空白默认布局，推荐改用：  
If you want to start RViz with the project preset instead of a blank default layout, prefer:

```bash
./scripts/run_rviz_preset.sh
```

如果你想带自定义分组规则一起启动 demo，可以运行：  
If you want to launch the demo together with custom grouping rules, run:

```bash
./scripts/run_demo_stack.sh --with-rules "/absolute/path/to/topic_grouping_rules.ini"
```

## 5. 添加面板 | Add the Panel

如果你是通过 `./scripts/run_rviz_preset.sh` 或 `./scripts/run_demo_stack.sh` 启动，通常不需要手动添加面板。  
If you started through `./scripts/run_rviz_preset.sh` or `./scripts/run_demo_stack.sh`, you usually do not need to add the panel manually.

如果你是通过 `./scripts/run_rviz_local.sh` 启动，则在 RViz 中执行：  
If you started through `./scripts/run_rviz_local.sh`, then inside RViz:

1. 打开 `Panels -> Add New Panel`  
   Open `Panels -> Add New Panel`
2. 选择 `rviz_auto_sensor_panel / SensorTreePanel`  
   Select `rviz_auto_sensor_panel / SensorTreePanel`

预期结果：  
Expected result:

- 你会看到 `Lidar`、`Camera`、`Point Cloud`、`Navigation`、`Visualization` 五个分类  
  You should see `Lidar`, `Camera`, `Point Cloud`, `Navigation`, and `Visualization` categories
- 每个分类下会进一步显示更友好的设备组，例如 `Front Lidar`、`Left Camera`  
  Each category should further show friendlier device groups such as `Front Lidar` and `Left Camera`
- 设备组下面的 topic 行会优先显示叶子名，例如 `scan`、`image_raw`、`points`  
  Topic rows under each device group should prefer leaf labels such as `scan`, `image_raw`, and `points`
- 演示 topics 会自动出现在树里  
  The demo topics should appear automatically in the tree

建议把 `Global Options -> Fixed Frame` 设置为 `map`。  
It is recommended to set `Global Options -> Fixed Frame` to `map`.

## 6. 检测插件是否工作 | How to Verify It Works

勾选这些 topics 来验证：  
Check these topics to validate behavior:

- `/demo/front/scan`
- `/demo/camera/left/image_raw`
- `/demo/lidar/points`
- `/demo/localization/odom`
- `/demo/navigation/path`
- `/demo/debug/markers`

预期结果：  
Expected result:

- RViz 左侧 Displays 区域会自动增加 `[Auto]` 开头的 display  
  RViz Displays should automatically gain displays prefixed with `[Auto]`
- 不需要手动 `Add` `LaserScan`、`Image`、`PointCloud2`、`Odometry`、`Path` 或 `MarkerArray`  
  You do not need to manually add `LaserScan`, `Image`, `PointCloud2`, `Odometry`, `Path`, or `MarkerArray`
- 取消勾选时，该 display 会被禁用  
  Unchecking disables the display
- 面板 `State` 列会显示 publisher 数量；离线保留的 topic 会显示 `Offline`  
  The panel `State` column shows publisher counts; retained offline topics are shown as `Offline`

当前面板还支持这些更省操作的交互：  
The panel now also supports these lower-effort interactions:

- `Auto-enable new topics`：新发现的支持 topic 自动创建 display  
  `Auto-enable new topics`: automatically create displays for newly discovered supported topics
- 顶部搜索框：按 topic 名、设备组名或消息类型快速过滤  
  Top search box: quickly filter by topic name, device-group label, or message type
- `Show offline`：切换是否显示已记住但当前离线的 topic  
  `Show offline`: toggle whether remembered-but-currently-offline topics stay visible
- `Enable All`：一次启用所有已发现 topic  
  `Enable All`: enable all discovered topics at once
- `Disable All`：一次关闭所有已发现 topic  
  `Disable All`: disable all discovered topics at once
- 设备组和分类节点也可以直接勾选，用来批量控制其下所有 topic  
  Device-group and category nodes can also be checked directly to control all descendant topics in one action
- 用户手动关闭过的 topic 会被记住，不会因为后续刷新立即自动打开  
  Topics manually turned off by the user are remembered and should not immediately auto-enable again on refresh

## 7. 如果面板没出现 | If the Panel Does Not Appear

请依次检查：  
Check these in order:

1. `./scripts/install_local.sh` 是否执行成功  
   Whether `./scripts/install_local.sh` succeeded
2. 是否通过 `./scripts/run_rviz_local.sh` 启动 RViz  
   Whether RViz was started through `./scripts/run_rviz_local.sh`
3. `rviz_common_plugins.xml` 是否已安装到 `install/share/rviz_auto_sensor_panel/`  
   Whether `rviz_common_plugins.xml` was installed to `install/share/rviz_auto_sensor_panel/`

## 8. 自定义分组规则 | Custom Grouping Rules

如果你想覆盖默认的 topic 分组规则，可以设置环境变量：  
If you want to override the default topic-grouping rules, you can set this environment variable:

```bash
export RVIZ_AUTO_SENSOR_PANEL_RULES_FILE="/absolute/path/to/topic_grouping_rules.ini"
```

默认规则文件样例在这里：  
The default rules example is here:

- `config/topic_grouping_rules.ini`

如果你想直接用一个示例覆盖文件启动，可以运行：  
If you want to launch directly with an example override file, run:

```bash
./scripts/run_rviz_with_rules.sh
```

或者显式指定某个规则文件：  
Or explicitly choose a rules file:

```bash
./scripts/run_rviz_with_rules.sh "/absolute/path/to/topic_grouping_rules.ini"
```

## 9. 当前最短路径 | Shortest Current Path

如果你只想最快看到效果，当前最短流程就是：  
If you just want the fastest path to seeing it work, the shortest current flow is:

```bash
./scripts/rebuild_install_local.sh
```

终端 1：  
Terminal 1:

```bash
./scripts/run_demo_publisher.sh
```

终端 2：  
Terminal 2:

```bash
./scripts/run_rviz_local.sh
```

终端 3，可选检查：  
Terminal 3, optional verification:

```bash
./scripts/check_demo_topics.sh
```

如果你只是想跑一次本地冒烟验证，不打开 RViz，可以执行：  
If you only want to run a local smoke test without opening RViz, run:

```bash
./scripts/smoke_test_local.sh
```

## 10. 可直接复制的验证命令 | Copy-Paste Validation Commands

如果你需要我帮你验证时，这一组命令就是之后默认可复制的标准流程：  
When I ask you to validate in future, this is the default copy-paste flow I will refer to:

终端 1：

```bash
cd "/Users/young/Documents/metropolia/26spring/multidisciplinary/rivz开发"
./scripts/rebuild_install_local.sh
./scripts/run_demo_publisher.sh
```

终端 2：

```bash
cd "/Users/young/Documents/metropolia/26spring/multidisciplinary/rivz开发"
./scripts/run_rviz_local.sh
```

终端 3：

```bash
cd "/Users/young/Documents/metropolia/26spring/multidisciplinary/rivz开发"
./scripts/check_demo_topics.sh
```

如果你要验证自定义规则覆盖，终端 2 可改成：  
If you want to validate custom rule overrides, Terminal 2 can instead be:

```bash
cd "/Users/young/Documents/metropolia/26spring/multidisciplinary/rivz开发"
./scripts/run_rviz_with_rules.sh
```
