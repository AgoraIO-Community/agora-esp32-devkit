# mybot-esp32

[![CI](https://github.com/junlon2006/mybot-esp32/actions/workflows/ci.yml/badge.svg)](https://github.com/junlon2006/mybot-esp32/actions/workflows/ci.yml)
[![License](https://img.shields.io/github/license/junlon2006/mybot-esp32)](LICENSE)

**[English](README.md) | [简体中文](README.zh-CN.md)**

`mybot-esp32` 是基于 ESP-IDF 的 ESP32-S3 固件工程，用于运行 MyBot 语音对话运行时。
工程负责板级初始化、Wi-Fi 配网、持久化存储、HTTPS、音频、输入、显示，以及 SDK 外围的
固件生命周期。

当前开发基线为 **ESP-IDF v5.5.2**，支持 13 个编译期 Board profile。通过 `MYBOT_BOARD`
选择板型、Flash、PSRAM、分区表和硬件驱动。具体硬件配置、输入操作、构建命令和限制见
[板卡说明索引](components/mybot_platform/boards/README.md)。

> 除非文件另有声明，本工程自维护代码使用 Apache-2.0。仓库内依赖和媒体资源有各自的
> 许可条款；重新分发源码或固件前请阅读[许可证与依赖](#许可证与依赖)。

## 当前能力

- Wi-Fi STA 自动重连与首次启动配网。
- 使用 NVS 持久化设备凭据和扬声器音量。
- 通过 `esp-tls`、系统 CA bundle、SNI 与 hostname 校验实现 HTTPS。
- 16 kHz 单声道 signed-16 PCM 采集与播放；随附 RTSA 使用 60 ms 音频包，其他包长需要
  匹配的 RTSA 构建。
- Agora RTSA 全双工音频、Cloud AEC、AI QoS、RTM 频道订阅与声纹状态显示。
- 所有 Board profile 使用共享的有界 PCM 播放缓冲和独立 I2S 播放任务。
- 带屏板型自动启用 LVGL，CoreS3 使用像素眼镜视图。
- 提供中英文配对码与 Wi-Fi 配网提示，并在配网页面显示实际热点名称。

## 支持的板卡

带屏 profile：

`zhengchen-1.54tft-ml307`、`zhengchen-1.54tft-wifi`、
`xingzhi-cube-1.54tft-wifi`、`bread-compact-wifi-lcd`、`esp-vocat`、
`esp32-s3-touch-amoled-1.75`、`esp32-s3-touch-amoled-1.75c`、
`esp32-s3-touch-amoled-2.16`、`m5stack-core-s3`、`m5stack-stick-s3` 和
`sensecap-watcher`。

无屏 profile：

`atom-echos3r` 和 `respeaker-flex-xvf3800-circular4-xiao`。

所有 profile 当前均使用 Wi-Fi。ML307 profile 仅预留 Modem UART 引脚，未实现 Modem 或
4G 联网。目前仅 `m5stack-core-s3` 提供摄像头视频能力。

## 构建

按照[官方安装指南](https://docs.espressif.com/projects/esp-idf/zh_CN/v5.5.2/esp32s3/get-started/index.html)
安装 ESP-IDF v5.5.2，并在当前 shell 中激活：

```sh
. /path/to/esp-idf/export.sh
test "$(idf.py --version)" = "ESP-IDF v5.5.2"
```

使用独立构建目录和 sdkconfig 构建单个板型：

```sh
idf.py -B build/<board-id> \
  -DMYBOT_BOARD=<board-id> \
  -DSDKCONFIG=build/<board-id>/sdkconfig \
  -DSDKCONFIG_DEFAULTS="sdkconfig.defaults;ci/ptime60.defaults" build
```

构建单个板型的中英文固件，或构建全部板型：

```sh
python3 scripts/build_all.py --board <board-id> --language both
python3 scripts/build_all.py
```

脚本将中间构建放在 `build/`，将合并镜像放在 `releases/`；带屏和视频能力由 Board profile
自动推导，使用随附 RTSA 的 60 ms 音频包，并为 CoreS3 默认开启视频。可使用
`--video off` 构建无视频对照，`--dry-run` 仅检查命令和路径，`--no-clean` 用于增量构建。

构建与输出根目录必须专用、互不嵌套，路径不得包含 `..` 或符号链接分量。常规运行会清理
由脚本管理的目录；已有非空目录若没有匹配的 `.mybot-generated-root.json` 标记会被拒绝。
安全清理要求 Python 3.11 或更高版本。各板直接构建和烧录命令见
[板卡说明索引](components/mybot_platform/boards/README.md)。

SenseCAP Watcher 首次烧录前必须备份出厂数据分区，并按其
[板卡说明](components/mybot_platform/boards/sensecap-watcher/README.md)使用
`idf.py flash`；禁止对该设备执行 `erase-flash`。

## 配网与控制

NVS 中没有 Wi-Fi 凭据时，设备创建名为 `mybot-xxxx` 的配置 AP，并在
`http://192.168.4.1` 提供配网页面。STA 获取可用 IP 后才启动 MyBot。请求配网时，固件先
停止 MyBot，联网后重新启动。

各板通过短按或触摸开始/结束对话，通过长按 3 秒请求配网；带音量控制的板型会将音量保存
到 NVS。具体输入和显示行为见
[板卡说明索引](components/mybot_platform/boards/README.md)。

使用 `idf.py -B <build-dir> menuconfig` 的 `mybot` 菜单选择产品语言、音频包长、Cloud AEC
与 AI QoS。设备服务地址随语言选择：

- 中文（`zh-CN`）：`https://botstation.shengwang.cn/api`
- 英文（`en-US`）：`https://botstation.agora.io/api`

可设置 `CONFIG_MYBOT_DEBUG_RESOURCE_MONITOR=y` 启用 CPU、堆和播放诊断，默认关闭。
播放计数器及解读见[音频播放缓冲](docs/AUDIO_PLAYBACK.zh-CN.md)。

## 验证

构建覆盖、已记录的真机结果和未支持能力统一维护在
[验证矩阵](docs/VALIDATION.md)中。CI 构建和宿主测试不能证明屏幕行为、上电时序、音频质量、
Wi-Fi 恢复等真机结果。

## 文档

- [板卡说明](components/mybot_platform/boards/README.md)
- [验证矩阵](docs/VALIDATION.md)
- [板级移植](docs/BOARD_PORTING.zh-CN.md)
- [音频播放缓冲](docs/AUDIO_PLAYBACK.zh-CN.md)
- [平台 LVGL UI](docs/PLATFORM_UI.zh-CN.md)
- [CoreS3 LVGL UI](docs/CORES3_UI.zh-CN.md)
- [CoreS3 音频播放对照测试](docs/CORES3_AUDIO_TEST.zh-CN.md)
- [CoreS3 摄像头上行](docs/CORES3_VIDEO.zh-CN.md)
- [发布检查清单](docs/RELEASING.zh-CN.md)
- [参与贡献](CONTRIBUTING.zh-CN.md)
- [支持](SUPPORT.zh-CN.md)

## 许可证与依赖

除非文件带有其他 SPDX 标识，本工程自维护代码使用 [Apache-2.0](LICENSE)。仓库还包含
采用独立许可证或分发条款的依赖和媒体资源，包括 AOSL 与 Agora RTSA。使用或重新分发前
请阅读 [VENDORED_SOURCES.md](VENDORED_SOURCES.md) 与
[THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md)。
