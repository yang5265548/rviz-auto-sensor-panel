# Contributing

中文 | English

感谢你关注 `rviz-auto-sensor-panel`。  
Thanks for your interest in `rviz-auto-sensor-panel`.

当前项目仍处于源码版 MVP 阶段，所以最重要的是保持：  
The project is still in a source-deliverable MVP stage, so the most important goals are:

- 功能改动小步推进  
  Keep changes small and incremental
- 文档和实现同步更新  
  Keep docs and implementation in sync
- 优先不破坏现有 demo 与本地验证链  
  Prefer not to break the existing demo and local validation flow

## 提交前建议 | Before You Submit

建议先运行：  
It is recommended to run:

```bash
./scripts/build_local.sh
./scripts/test_local.sh
./scripts/install_local.sh
```

如果改动涉及启动链、预设或 demo，也建议补跑：  
If your change affects startup flow, presets, or the demo, also run:

```bash
./scripts/smoke_test_local.sh
```

## 分支建议 | Branching

- `main`：保持稳定  
  `main`: keep stable
- `codex/<topic>`：短生命周期功能分支  
  `codex/<topic>`: short-lived feature branches

## 文档要求 | Documentation

当前仓库默认使用**中英双语文档**。  
This repository currently defaults to **bilingual Chinese/English documentation**.

如果你新增或明显修改以下内容，请同步更新文档：  
If you add or significantly change any of the following, update the docs as well:

- 启动方式  
  Launch flow
- 安装方式  
  Installation flow
- 规则文件  
  Rules files
- 用户可见交互  
  User-visible interactions

## 代码风格 | Code Style

- C++17
- 优先可读性，不追求炫技式写法  
  Prefer readability over cleverness
- 避免复杂模板技巧  
  Avoid complex template tricks
- 新增逻辑尽量配套测试  
  Add tests for new logic whenever practical

## 提交说明 | Commit Guidance

提交信息尽量直接描述变更结果，例如：  
Try to keep commit messages outcome-oriented, for example:

- `Add user install flow and RViz preset`
- `Fix auto display state sync`
- `Reduce sensor panel minimum width pressure`
