# 支持

可复现的 bug 与明确的特性请求请使用 GitHub Issues。GitHub Discussions 开启后，集成问题请
在那里讨论。

> [English](SUPPORT.md) | [简体中文](SUPPORT.zh-CN.md)

请提供：

- 准确的仓库 commit 与 `idf.py --version` 输出；
- Board profile，以及问题发生在构建、启动、配网、配对还是 RTC 阶段；
- 已删除凭据、token 和设备/客户数据的串口日志；
- Flash/PSRAM 配置和任何硬件改动；
- 相关配置，包括语言、视频、LVGL 主题/动画和资源监控开关；
- 复现步骤与最后一个正常版本。

显示问题请附上屏幕照片，并说明问题是在真机还是仅在主机 UI 测试中出现。所有带屏板型自动
使用 LVGL，不存在可切换的旧 UI 后端；AtomEchoS3R 与 ReSpeaker Flex 没有显示屏。配网问题请注明发生在
首次启动、长按触发，还是使用已保存凭据重新连接时。

支持的 profile 与硬件前提见[板卡说明索引](components/mybot_platform/boards/README.md)。
当前构建与真机状态统一记录在[验证矩阵](docs/VALIDATION.md)中。

ESP-VoCat 问题必须注明 PCB V1.0 或 V1.2，并附带运行时探测日志。三个 AMOLED profile 必须
使用匹配的 profile，禁止交叉烧录。Bread Compact Wi-Fi LCD 问题需注明 N16R8 模组、
240x320 ST7789 版本和实际接线。在实现并验证兼容网络路径之前，ML307/4G 相关问题按特性
请求处理。

随附 RTSA 包支持 60 ms 音频帧。选择 20 ms 或 40 ms 导致配置被拒绝时，需要提供匹配的
RTSA 包，而不是修改板级驱动。

本项目不提供支持 SLA。Agora 商业 SDK 或云服务问题应通过对应的 Agora 支持渠道处理。
