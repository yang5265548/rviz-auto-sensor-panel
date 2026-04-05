# Quick Start

## 1. 文档目标 | Document Goal

本文档给出当前项目最直接的本地启动方式，目标是尽快把插件跑起来并看到自动发现效果。  
This document provides the most direct local startup path for the current project, with the goal of getting the plugin running and showing auto-discovery as quickly as possible.

## 2. 一次性准备 | One-Time Setup

在项目根目录执行：  
Run these from the project root:

```bash
python3 -m pip install --target ./vendor -r requirements-dev.txt
./scripts/configure_local.sh
./scripts/build_local.sh
./scripts/install_local.sh
```

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

## 4. 启动 RViz | Start RViz

打开第二个终端，source ROS 2 环境，并让 RViz 能找到本地安装结果。  
Open a second terminal, source ROS 2, and make sure RViz can discover the local install.

说明：当前仓库使用的是 package 级安装结果，因此脚本会加载  
`install/share/rviz_auto_sensor_panel/local_setup.sh`。  
Note: this repository currently uses package-level install output, so the script loads  
`install/share/rviz_auto_sensor_panel/local_setup.sh`.

建议顺序：  
Suggested order:

```bash
./scripts/run_rviz_local.sh
```

## 5. 添加面板 | Add the Panel

在 RViz 中：  
Inside RViz:

1. 打开 `Panels -> Add New Panel`  
   Open `Panels -> Add New Panel`
2. 选择 `rviz_auto_sensor_panel / SensorTreePanel`  
   Select `rviz_auto_sensor_panel / SensorTreePanel`

预期结果：  
Expected result:

- 你会看到 `Lidar`、`Camera`、`Point Cloud` 三个分类  
  You should see `Lidar`, `Camera`, and `Point Cloud` categories
- 演示 topics 会自动出现在树里  
  The demo topics should appear automatically in the tree

## 6. 检测插件是否工作 | How to Verify It Works

勾选这些 topics 来验证：  
Check these topics to validate behavior:

- `/demo/front/scan`
- `/demo/camera/left/image_raw`
- `/demo/lidar/points`

预期结果：  
Expected result:

- RViz 左侧 Displays 区域会自动增加 `[Auto]` 开头的 display  
  RViz Displays should automatically gain displays prefixed with `[Auto]`
- 不需要手动 `Add` `LaserScan`、`Image`、`PointCloud2`  
  You do not need to manually add `LaserScan`, `Image`, or `PointCloud2`
- 取消勾选时，该 display 会被禁用  
  Unchecking disables the display

当前面板还支持这些更省操作的交互：  
The panel now also supports these lower-effort interactions:

- `Auto-enable new topics`：新发现的支持 topic 自动创建 display  
  `Auto-enable new topics`: automatically create displays for newly discovered supported topics
- `Enable All`：一次启用所有已发现 topic  
  `Enable All`: enable all discovered topics at once
- `Disable All`：一次关闭所有已发现 topic  
  `Disable All`: disable all discovered topics at once

## 7. 如果面板没出现 | If the Panel Does Not Appear

请依次检查：  
Check these in order:

1. `./scripts/install_local.sh` 是否执行成功  
   Whether `./scripts/install_local.sh` succeeded
2. 是否通过 `./scripts/run_rviz_local.sh` 启动 RViz  
   Whether RViz was started through `./scripts/run_rviz_local.sh`
3. `rviz_common_plugins.xml` 是否已安装到 `install/share/rviz_auto_sensor_panel/`  
   Whether `rviz_common_plugins.xml` was installed to `install/share/rviz_auto_sensor_panel/`

## 8. 当前最短路径 | Shortest Current Path

如果你只想最快看到效果，当前最短流程就是：  
If you just want the fastest path to seeing it work, the shortest current flow is:

```bash
python3 -m pip install --target ./vendor -r requirements-dev.txt
./scripts/configure_local.sh
./scripts/build_local.sh
./scripts/install_local.sh
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
