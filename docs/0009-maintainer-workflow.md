# Maintainer Workflow

## 1. 文档目标 | Document Goal

本文档面向仓库维护者，说明当前推荐的本地开发、验证、合并和发布节奏。  
This document is for repository maintainers and describes the recommended local development, validation, merge, and release flow.

## 2. 日常开发流程 | Day-to-Day Flow

推荐顺序：  
Recommended order:

1. 从 `main` 拉出短生命周期分支  
   Create a short-lived branch from `main`
2. 小步提交  
   Commit in small increments
3. 运行本地验证链  
   Run the local validation chain
4. 更新双语文档  
   Update bilingual documentation
5. 合并回 `main`  
   Merge back to `main`

## 3. 当前推荐命令 | Current Recommended Commands

基础开发验证：  
Base development validation:

```bash
./scripts/build_local.sh
./scripts/test_local.sh
./scripts/install_local.sh
```

如果涉及 demo、预设或启动链：  
If the change affects demo, preset, or startup flow:

```bash
./scripts/smoke_test_local.sh
```

如果怀疑环境有问题：  
If you suspect environment issues:

```bash
./scripts/doctor_local.sh
```

如果想清理项目内运行时状态：  
If you want to reset project-local runtime state:

```bash
./scripts/clean_local_state.sh
```

## 4. 何时更新哪些文档 | When to Update Which Docs

- 功能范围变化：更新 [`docs/0001-rviz-auto-sensor-panel-mvp.md`](/Users/young/Documents/metropolia/26spring/multidisciplinary/rivz开发/docs/0001-rviz-auto-sensor-panel-mvp.md)  
  Scope changes: update [`docs/0001-rviz-auto-sensor-panel-mvp.md`](/Users/young/Documents/metropolia/26spring/multidisciplinary/rivz开发/docs/0001-rviz-auto-sensor-panel-mvp.md)
- 架构调整：更新 [`docs/0002-rviz-auto-sensor-panel-architecture.md`](/Users/young/Documents/metropolia/26spring/multidisciplinary/rivz开发/docs/0002-rviz-auto-sensor-panel-architecture.md)  
  Architecture changes: update [`docs/0002-rviz-auto-sensor-panel-architecture.md`](/Users/young/Documents/metropolia/26spring/multidisciplinary/rivz开发/docs/0002-rviz-auto-sensor-panel-architecture.md)
- 启动/安装变化：更新 [`docs/0005-quick-start.md`](/Users/young/Documents/metropolia/26spring/multidisciplinary/rivz开发/docs/0005-quick-start.md) 和 [`docs/0007-user-installation.md`](/Users/young/Documents/metropolia/26spring/multidisciplinary/rivz开发/docs/0007-user-installation.md)  
  Startup/install changes: update [`docs/0005-quick-start.md`](/Users/young/Documents/metropolia/26spring/multidisciplinary/rivz开发/docs/0005-quick-start.md) and [`docs/0007-user-installation.md`](/Users/young/Documents/metropolia/26spring/multidisciplinary/rivz开发/docs/0007-user-installation.md)
- 发布状态变化：更新 [`CHANGELOG.md`](/Users/young/Documents/metropolia/26spring/multidisciplinary/rivz开发/CHANGELOG.md) 和 [`docs/0008-release-readiness.md`](/Users/young/Documents/metropolia/26spring/multidisciplinary/rivz开发/docs/0008-release-readiness.md)  
  Release-status changes: update [`CHANGELOG.md`](/Users/young/Documents/metropolia/26spring/multidisciplinary/rivz开发/CHANGELOG.md) and [`docs/0008-release-readiness.md`](/Users/young/Documents/metropolia/26spring/multidisciplinary/rivz开发/docs/0008-release-readiness.md)

## 5. 当前发布建议 | Current Release Guidance

当前版本更适合按“源码可交付 MVP”发布，而不是系统包级发布。  
The current version is best treated as a "source-deliverable MVP" rather than a system-package release.

更适合的发布动作是：  
The better current release actions are:

- 打 tag  
  Tag a version
- 更新 changelog  
  Update the changelog
- 在 README 和用户指南中写清楚使用边界  
  Clearly state usage boundaries in the README and user guide

## 6. 当前主要风险 | Current Main Risks

- RViz dock 行为在不同机器上可能仍有差异  
  RViz dock behavior may still differ across machines
- 目前只覆盖三类消息类型  
  Only three message types are covered
- 目前没有 CI  
  There is currently no CI
