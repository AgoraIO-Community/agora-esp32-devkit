# Board documentation / 板卡说明

These documents describe the board profiles implemented by `mybot-esp32`. Pins, power sequencing,
drivers and build constraints are defined by the checked-in `board.cmake`, `board_config.h` and
`sdkconfig.defaults` files. Current validation status is centralized in
[`docs/VALIDATION.md`](../../../docs/VALIDATION.md).

以下说明对应本工程已实现的 16 个板型。硬件配置、输入行为和构建约束以当前板级代码为准；
验证状态统一见[验证矩阵](../../../docs/VALIDATION.md)。不同尺寸、芯片或硬件版本的板卡
不能仅凭外观共用固件。

| Board profile | 板卡说明 | 显示 | 视频 |
| --- | --- | --- | --- |
| `atom-echos3r` | [README](atom-echos3r/README.md) | 无屏幕 | 无 |
| `bread-compact-wifi-lcd` | [README](bread-compact-wifi-lcd/README.md) | 240×320 ST7789 | 无 |
| `esp-vocat` | [README](esp-vocat/README.md) | ST77916 触摸屏 | 无 |
| `esp32-s3-touch-amoled-1.75` | [README](esp32-s3-touch-amoled-1.75/README.md) | CO5300 AMOLED 触摸屏 | 无 |
| `esp32-s3-touch-amoled-1.75c` | [README](esp32-s3-touch-amoled-1.75c/README.md) | CO5300 AMOLED 触摸屏 | 无 |
| `esp32-s3-touch-amoled-2.16` | [README](esp32-s3-touch-amoled-2.16/README.md) | 480×480 CO5300 AMOLED 触摸屏 | 无 |
| `esp32-s3-touch-amoled-1.8-v2` | [README](esp32-s3-touch-amoled-1.8-v2/README.md) | 368×448 CO5300 AMOLED 触摸屏 | 无 |
| `esp32-s3-touch-lcd-1.85` | [README](esp32-s3-touch-lcd-1.85/README.md) | 360×360 ST77916 触摸屏 | 无 |
| `genjutech-s3-1.54tft` | [README](genjutech-s3-1.54tft/README.md) | 240×240 ST7789 | 无 |
| `m5stack-core-s3` | [README](m5stack-core-s3/README.md) | ILI9342 触摸屏 | GC0308，上传 1 fps |
| `m5stack-stick-s3` | [README](m5stack-stick-s3/README.md) | 135×240 ST7789P3 | 无 |
| `respeaker-flex-xvf3800-circular4-xiao` | [README](respeaker-flex-xvf3800-circular4-xiao/README.md) | 无屏幕 | 无 |
| `sensecap-watcher` | [中文](sensecap-watcher/README.md) / [English](sensecap-watcher/README_en.md) | SPD2010 | 无 |
| `xingzhi-cube-1.54tft-wifi` | [README](xingzhi-cube-1.54tft-wifi/README.md) | 240×240 ST7789 | 无 |
| `zhengchen-1.54tft-ml307` | [README](zhengchen-1.54tft-ml307/README.md) | ST7789 | 无 |
| `zhengchen-1.54tft-wifi` | [README](zhengchen-1.54tft-wifi/README.md) | ST7789 | 无 |

## Build, flash and validation / 构建、烧录与验证

Activate ESP-IDF v5.5.2 from the repository root:

```sh
. /path/to/esp-idf/export.sh
test "$(idf.py --version)" = "ESP-IDF v5.5.2"
```

Build both languages for one profile:

```sh
python3 scripts/build_all.py --board <board-id> --language both
```

The script writes managed builds to `build/` and merged images to `releases/`, derives display
and video capability from the profile, uses the bundled 60 ms RTSA cadence and enables CoreS3
video by default. Use `--video off` for a no-video comparison. See the
[project README](../../../README.md#build) for root ownership and cleanup rules.

For a direct build and flash, use a dedicated build directory and port:

```sh
idf.py -B build/<board-id> \
  -DMYBOT_BOARD=<board-id> \
  -DSDKCONFIG=build/<board-id>/sdkconfig \
  -DSDKCONFIG_DEFAULTS="sdkconfig.defaults;ci/ptime60.defaults" build
idf.py -B build/<board-id> -p PORT flash monitor
```

Add `ci/en-us.defaults` for English. Add `ci/video.defaults` only when building CoreS3 with
video. SenseCAP Watcher requires the factory-partition backup procedure in its board README.

Board input, provisioning and hardware-specific prerequisites remain in each board README.
Completed build and hardware results are tracked only in the
[validation matrix](../../../docs/VALIDATION.md).

设备首次启动无 Wi-Fi 凭据时自动配网；长按配网输入会先停止 mybot，再进入配网流程，
联网后重新启动 mybot。各板的配网输入、短按/触摸和音量输入见对应 README。
