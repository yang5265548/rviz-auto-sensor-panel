# Development Setup

## 1. 文档目标 | Document Goal

本文档记录当前仓库在本地开发、构建和测试时需要的最小环境与命令。  
This document records the minimum environment and commands needed to develop, build, and test this repository locally.

## 2. 当前验证环境 | Current Verified Environment

- 操作系统：macOS  
  Operating system: macOS
- ROS 2：`jazzy`  
  ROS 2: `jazzy`
- Qt：`qt@5`（Homebrew）  
  Qt: `qt@5` (Homebrew)
- 构建工具：`cmake`  
  Build tool: `cmake`

说明：  
Notes:

- 当前环境里没有 `colcon`，所以现在以 `cmake` 构建为主。  
  `colcon` is not currently available in this environment, so the project is built with `cmake` for now.
- ROS 2 的 `ament_cmake` 在当前机器上依赖 Python 包 `ament_package` 与 `catkin_pkg`。  
  ROS 2 `ament_cmake` depends on Python packages such as `ament_package` and `catkin_pkg` on this machine.
- `ament_package` 已经存在于本机 ROS 安装里，`catkin_pkg` 通过项目本地 `vendor/` 提供。  
  `ament_package` already exists in the local ROS installation, and `catkin_pkg` is provided via the project's local `vendor/`.

## 3. 本地 Python 依赖 | Local Python Dependency

首次配置前，请在项目根目录执行：  
Before the first configure step, run this from the project root:

```bash
python3 -m pip install --target ./vendor -r requirements-dev.txt
```

## 4. 构建步骤 | Build Steps

推荐使用仓库自带脚本：  
Recommended: use the repository scripts:

```bash
./scripts/configure_local.sh
./scripts/build_local.sh
./scripts/install_local.sh
```

运行时推荐使用：  
Recommended runtime scripts:

```bash
./scripts/run_demo_publisher.sh
./scripts/run_rviz_local.sh
```

这些脚本会自动为当前项目设置需要的 `PYTHONPATH`。  
These scripts automatically set the `PYTHONPATH` needed by the project.

## 5. 测试步骤 | Test Steps

```bash
./scripts/test_local.sh
```

当前已接入测试：  
Currently integrated tests:

- `sensor_classifier_test`

## 6. 已知环境注意事项 | Known Environment Notes

- 如果 CMake 提示找不到 `Qt5Config.cmake`，请确认 Homebrew 的 `qt@5` 已安装。  
  If CMake cannot find `Qt5Config.cmake`, make sure Homebrew `qt@5` is installed.
- 当前仓库的 `CMakeLists.txt` 已优先加入 `/opt/homebrew/opt/qt@5`。  
  The current `CMakeLists.txt` already prioritizes `/opt/homebrew/opt/qt@5`.
- 链接阶段可能看到一些 macOS SDK 版本警告，但当前并未阻止构建通过。  
  You may see some macOS SDK version warnings during linking, but they are not currently blocking successful builds.

## 7. 下一步建议 | Recommended Next Steps

- 增加更多单元测试  
  Add more unit tests
- 补一个最小 RViz 手工验证流程  
  Add a minimal manual RViz verification flow
- 等 `colcon` 可用后补充标准 ROS 2 工作流说明  
  Add standard ROS 2 workflow instructions once `colcon` is available
