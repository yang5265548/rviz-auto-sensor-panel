# User Installation Guide

## 1. 文档目标 | Document Goal

本文档面向“使用插件的人”，而不是开发插件的人。  
This document is for people who want to use the plugin, not develop it.

目标是把安装、启动和第一次使用的步骤压缩到最少。  
The goal is to minimize the steps needed for installation, launch, and first use.

## 2. 当前交付形态 | Current Delivery Format

当前项目提供的是：  
The project currently ships as:

- 一个可从源码构建的 ROS 2 package  
  A ROS 2 package that can be built from source
- 本地安装脚本  
  A local installation script
- 默认 RViz 预设  
  A default RViz preset

当前项目还没有发布系统级二进制安装包。  
The project does not yet ship as a system-wide binary package.

## 3. 前置条件 | Preconditions

- 已安装 ROS 2 Jazzy  
  ROS 2 Jazzy is installed
- 本机可运行 `rviz2`  
  `rviz2` runs on the machine
- 已克隆本仓库  
  This repository has been cloned
- Linux 上建议确保已安装 `python3-pip`，或者至少安装 `python3-catkin-pkg`  
  On Linux, it is recommended to have `python3-pip` installed, or at minimum `python3-catkin-pkg`

如果你的 ROS 2 不在默认位置，先设置：  
If your ROS 2 install is not in a default location, set this first:

```bash
export RVIZ_AUTO_SENSOR_PANEL_ROS_SETUP=/absolute/path/to/setup.bash
```

## 4. 安装命令 | Installation Command

在项目根目录执行：  
Run this from the project root:

```bash
./scripts/install_user_local.sh
```

这个脚本会：  
This script will:

- 自动探测并加载常见的 ROS 2 安装  
  Auto-detect and load common ROS 2 installs
- 准备本地 Python 开发依赖  
  Prepare local Python development dependencies
- 配置、构建并安装 package  
  Configure, build, and install the package
- 给出下一步启动提示  
  Print the next launch steps

## 5. 启动插件 | Launch the Plugin

如果你已经有自己的 ROS 2 传感器 topic，在一个终端先启动你的系统。  
If you already have your own ROS 2 sensor topics, start your system in one terminal first.

然后在项目根目录执行：  
Then run this from the project root:

```bash
./scripts/run_rviz_preset.sh
```

这个预设会自动：  
This preset will automatically:

- 加载 `SensorTreePanel`
  Load `SensorTreePanel`
- 询问是否按 SLAM 模式启动
  Ask whether to start in SLAM mode
- 保留常用 RViz 基础面板  
  Keep the standard RViz base panels

如果你选择：
If you choose:

- `SLAM mode = yes` -> `Fixed Frame = map`
- `SLAM mode = no` -> `Fixed Frame = base_link`

如果你不想交互式选择，也可以直接指定：
If you do not want the interactive prompt, you can specify it directly:

```bash
./scripts/run_rviz_preset.sh --slam-mode
./scripts/run_rviz_preset.sh --no-slam-mode
./scripts/run_rviz_preset.sh --fixed-frame odom
```

## 6. 如果你只想快速试用 | If You Just Want a Quick Demo

直接执行：  
Just run:

```bash
./scripts/run_demo_stack.sh
```

它会启动 demo publisher，再用默认 RViz 预设打开 RViz。  
It starts the demo publisher and then opens RViz with the default preset.

## 7. 第一次使用时怎么操作 | First-Time Usage

启动后，你应该直接看到：  
After launch, you should directly see:

- `SensorTreePanel`
- `Lidar`、`Camera`、`Point Cloud`、`Map`、`Navigation`、`Visualization` 分类  
  `Lidar`, `Camera`, `Point Cloud`, `Map`, `Navigation`, and `Visualization` categories
- `Fixed Frame` 按启动选择决定  
  `Fixed Frame` set according to your startup choice

接下来只需要：  
Then you only need to:

1. 在树中勾选你想看的 topic  
   Check the topics you want to visualize
2. 观察 RViz 左侧 `Displays` 是否自动出现 `[Auto] ...` 项  
   Verify that `[Auto] ...` displays appear automatically in the RViz `Displays` panel

你还可以直接用面板顶部的搜索框筛选 topic，并通过 `Show offline` 控制是否显示当前离线但已记住的项。  
You can also use the top search box to filter topics directly and use `Show offline` to control whether currently offline remembered items remain visible.

## 8. 自定义分组规则 | Custom Grouping Rules

如果你的 topic 命名风格和默认规则不一致，可以用自定义规则文件启动：  
If your topic naming differs from the default rules, launch with a custom rules file:

```bash
./scripts/run_demo_stack.sh --with-rules "/absolute/path/to/topic_grouping_rules.ini"
```

或者仅对 RViz 预设启动做覆盖：  
Or override only the RViz preset launch:

```bash
RVIZ_AUTO_SENSOR_PANEL_RULES_FILE="/absolute/path/to/topic_grouping_rules.ini" ./scripts/run_rviz_preset.sh
```

## 9. 本地自检 | Local Smoke Check

如果你想快速确认“构建和 demo 启动链”没有坏，可以执行：  
If you want to quickly verify that the build and demo startup chain still works, run:

```bash
./scripts/smoke_test_local.sh
```

## 10. 当前限制 | Current Limitations

- 当前支持的消息类型仍是有限集合，不会自动接纳所有 ROS 2 topic  
  The supported message types are still a finite set; not every ROS 2 topic is auto-included
- 当前交付方式仍是源码安装  
  The current delivery mode is still source-based installation
- 某些环境下自动 topic 检查可能受 DDS 可见性影响  
  In some environments, automatic topic checks may be affected by DDS visibility
