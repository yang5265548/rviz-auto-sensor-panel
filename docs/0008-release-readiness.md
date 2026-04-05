# Release Readiness

## 1. 文档目标 | Document Goal

本文档记录当前仓库距离“正式对外发布版本”还有哪些事项。  
This document records what remains before the repository is ready for a formal public release.

## 2. 当前已完成 | Already Completed

- Package metadata (`package.xml`, `CMakeLists.txt`)  
  Package metadata (`package.xml`, `CMakeLists.txt`)
- 双语核心文档  
  Core bilingual documentation
- 本地安装脚本与 demo 启动链  
  Local install scripts and demo startup flow
- RViz 预设与用户安装指南  
  RViz preset and user installation guide
- 单元测试与本地 smoke test  
  Unit tests and a local smoke test
- 开源许可证与 changelog  
  Open-source license and changelog

## 3. 仍建议补充 | Still Recommended Before Wider Release

- 更准确的维护者邮箱和发布者信息  
  More accurate maintainer email and publisher metadata
- 在一台干净机器上做完整用户安装验证  
  Full user-install validation on a clean machine
- 增加更多传感器类型支持  
  Support for more sensor message types
- 增加更强的 RViz 行为回归验证  
  Stronger RViz behavior regression coverage
- 如果要正式对外发布，可增加 GitHub Actions CI  
  If releasing publicly, add GitHub Actions CI

## 4. 当前结论 | Current Conclusion

当前仓库已经达到“源码可交付 MVP”状态。  
The repository has reached a "source-deliverable MVP" state.

这意味着开发者用户已经可以：  
This means developer users can already:

- 克隆仓库  
  Clone the repository
- 本地安装  
  Install locally
- 启动插件  
  Launch the plugin
- 运行 demo 或接入自己的 topic  
  Run the demo or connect their own topics

但它还不等同于“系统包级正式发布”。  
But it is not yet the same as a formally released system package.
