# Main Merge Checklist

## 1. 文档目标 | Document Goal

本文档用于在把开发分支合并回 `main` 前，快速确认关键事项没有遗漏。  
This document is used to quickly confirm that no critical items were missed before merging a development branch back into `main`.

## 2. 合并前最小检查 | Minimum Pre-Merge Checks

在准备合并到 `main` 前，至少确认以下项目：  
Before merging into `main`, confirm at least the following:

1. 代码已提交干净  
   The working tree is clean
2. 本地构建通过  
   Local build passes
3. 本地测试通过  
   Local tests pass
4. 如果改动涉及启动链或 demo，本地 smoke test 已跑过  
   If the change affects startup flow or demo behavior, the local smoke test has been run
5. 双语文档已同步  
   Bilingual docs have been kept in sync
6. `CHANGELOG.md` 已按需要更新  
   `CHANGELOG.md` has been updated if needed

## 3. 推荐命令 | Recommended Commands

```bash
./scripts/preflight_local.sh
```

如果你只想手动执行最关键几步，也可以运行：  
If you prefer to run the key steps manually, you can also run:

```bash
./scripts/build_local.sh
./scripts/test_local.sh
./scripts/install_local.sh
./scripts/smoke_test_local.sh
```

## 4. 文档同步提醒 | Documentation Sync Reminder

如果你的改动涉及以下内容，请确认对应文档已更新：  
If your change touches any of the following, confirm that the matching docs were updated:

- 安装方式  
  Installation flow
- 启动方式  
  Launch flow
- 规则文件  
  Rules files
- 可见 UI 交互  
  User-visible UI behavior
- 发布状态  
  Release status

## 5. 当前建议 | Current Recommendation

在本项目当前阶段，更推荐：  
At the current stage of this project, it is recommended to:

- 先把 `codex/...` 功能分支整理干净  
  First keep `codex/...` feature branches tidy
- 确认本地验证链稳定  
  Confirm the local validation chain is stable
- 再合并到 `main`  
  Then merge to `main`
