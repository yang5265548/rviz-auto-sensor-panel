# Colleague Quick Guide

## 1. 文档目标 | Goal

这是一份发给同事即可直接复制执行的快速指导。  
This is a quick guide you can send directly to teammates for copy-paste use.

目标分成两种：  
There are two primary use cases:

- 先快速跑 demo，确认插件能工作  
  Quickly run the demo and confirm the plugin works
- 接入他们自己的 ROS 2 系统  
  Connect the plugin to their own ROS 2 system

## 2. 前置条件 | Preconditions

- 已安装 ROS 2 Jazzy  
  ROS 2 Jazzy is installed
- 已安装 `rviz2`  
  `rviz2` is installed
- 已安装 `git`  
  `git` is installed
- Linux 上建议安装 `python3-pip`，或者至少安装 `python3-catkin-pkg`  
  On Linux, it is recommended to install `python3-pip`, or at minimum `python3-catkin-pkg`

如果 ROS 2 不在默认路径，先设置：  
If ROS 2 is not installed in a default location, set this first:

```bash
export RVIZ_AUTO_SENSOR_PANEL_ROS_SETUP=/absolute/path/to/setup.bash
```

## 3. 最快试用 Demo | Fastest Demo Trial

直接复制执行：  
Copy and run:

```bash
git clone https://github.com/yang5265548/rviz-auto-sensor-panel.git
cd rviz-auto-sensor-panel
git checkout main
./scripts/install_user_local.sh
./scripts/run_demo_stack.sh
```

预期结果：  
Expected result:

- 自动打开 RViz  
  RViz opens automatically
- 自动加载 `SensorTreePanel`  
  `SensorTreePanel` is loaded automatically
- 自动把 `Fixed Frame` 设成 `map`  
  `Fixed Frame` is set to `map`
- 会看到 demo 的 `Lidar`、`Camera`、`Point Cloud`、`Navigation`、`Visualization` 数据  
  You will see demo `Lidar`, `Camera`, `Point Cloud`, `Navigation`, and `Visualization` data

## 4. 接入自己的 ROS 2 系统 | Use with Your Own ROS 2 System

如果你已经有自己的 ROS 2 节点和 topics 在运行，不要启动 demo，直接执行：  
If your own ROS 2 nodes and topics are already running, do not start the demo. Run this instead:

```bash
git clone https://github.com/yang5265548/rviz-auto-sensor-panel.git
cd rviz-auto-sensor-panel
git checkout main
./scripts/install_user_local.sh
./scripts/run_rviz_preset.sh
```

这个命令会：  
This command will:

- 直接打开带默认 preset 的 RViz  
  Open RViz with the default preset
- 自动加载 `SensorTreePanel`  
  Load `SensorTreePanel` automatically
- 询问是否是 SLAM 模式  
  Ask whether this is a SLAM session

选择规则：  
Selection rule:

- `Yes` -> `Fixed Frame = map`
- `No` -> `Fixed Frame = base_link`

如果你不想交互，可以直接指定：  
If you do not want the prompt, specify it directly:

```bash
./scripts/run_rviz_preset.sh --slam-mode
./scripts/run_rviz_preset.sh --no-slam-mode
./scripts/run_rviz_preset.sh --fixed-frame odom
```

直接在终端里执行 `./scripts/run_rviz_preset.sh` 时，默认会先询问；环境变量方式主要用于非交互脚本。  
When `./scripts/run_rviz_preset.sh` is run directly in a terminal, it prompts first by default; environment-variable control is mainly intended for non-interactive scripts.

## 5. 普通 RViz 模式，但保留插件 | Plain RViz with the Plugin

如果你想打开最普通的 RViz 界面，而不是加载项目 preset，可以执行：  
If you want a plain RViz window instead of the project preset, run:

```bash
./scripts/run_plain_rviz_with_plugin.sh
```

这个模式下：  
In this mode:

- 插件会被 RViz 发现  
  The plugin is available to RViz
- 但不会自动带出 `SensorTreePanel`  
  `SensorTreePanel` is not auto-added

需要手动添加：  
You need to add it manually:

1. `Panels -> Add New Panel`
2. 选择 `rviz_auto_sensor_panel / SensorTreePanel`

## 6. 当前自动支持的消息类型 | Currently Supported Message Types

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

只有这些类型会自动出现在面板里。  
Only these message types are automatically included in the panel.

## 7. 常见问题 | Common Issues

### 7.1 没有 `pip` | No `pip`

如果看到：
If you see:

```text
/usr/bin/python3: No module named pip
```

先执行：  
Run first:

```bash
sudo apt update
sudo apt install -y python3-pip
```

或者：  
Or:

```bash
sudo apt update
sudo apt install -y python3-catkin-pkg
```

### 7.2 ROS 2 不在默认路径 | ROS 2 Not in a Default Location

先设置：
Set this first:

```bash
export RVIZ_AUTO_SENSOR_PANEL_ROS_SETUP=/absolute/path/to/setup.bash
```

然后再执行安装和启动命令。  
Then rerun the install and launch commands.

### 7.3 想确认 demo 是否还在运行 | Check Whether the Demo Is Still Running

```bash
./scripts/check_demo_topics.sh
```

如果你想关掉 demo publisher：  
If you want to stop the demo publisher:

```bash
pkill -f demo_sensor_publisher || true
```
