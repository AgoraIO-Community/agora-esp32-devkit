# Board documentation / 板卡说明

These documents describe the board profiles implemented by `mybot-esp32`. Pins, power sequencing,
drivers and build constraints are defined by the checked-in `board.cmake`, `board_config.h` and
`sdkconfig.defaults` files. Use ESP-IDF v5.5.2 and select a board with `MYBOT_BOARD`.

以下说明对应本工程已实现的 13 个板型。硬件配置、输入行为、构建约束和验证状态以当前
板级代码及本工程文档为准。不同尺寸、芯片或硬件版本的板卡不能仅凭外观共用固件。

| Board profile | 板卡说明 | 显示 | 视频 |
| --- | --- | --- | --- |
| `atom-echos3r` | [README](atom-echos3r/README.md) | 无屏幕 | 无 |
| `bread-compact-wifi-lcd` | [README](bread-compact-wifi-lcd/README.md) | 240×320 ST7789 | 无 |
| `esp-vocat` | [README](esp-vocat/README.md) | ST77916 触摸屏 | 无 |
| `esp32-s3-touch-amoled-1.75` | [README](esp32-s3-touch-amoled-1.75/README.md) | CO5300 AMOLED 触摸屏 | 无 |
| `esp32-s3-touch-amoled-1.75c` | [README](esp32-s3-touch-amoled-1.75c/README.md) | CO5300 AMOLED 触摸屏 | 无 |
| `esp32-s3-touch-amoled-2.16` | [README](esp32-s3-touch-amoled-2.16/README.md) | 480×480 CO5300 AMOLED 触摸屏 | 无 |
| `m5stack-core-s3` | [README](m5stack-core-s3/README.md) | ILI9342 触摸屏 | GC0308，上传 1 fps |
| `m5stack-stick-s3` | [README](m5stack-stick-s3/README.md) | 135×240 ST7789P3 | 无 |
| `respeaker-flex-xvf3800-circular4-xiao` | [README](respeaker-flex-xvf3800-circular4-xiao/README.md) | 无屏幕 | 无 |
| `sensecap-watcher` | [中文](sensecap-watcher/README.md) / [English](sensecap-watcher/README_en.md) | SPD2010 | 无 |
| `xingzhi-cube-1.54tft-wifi` | [README](xingzhi-cube-1.54tft-wifi/README.md) | 240×240 ST7789 | 无 |
| `zhengchen-1.54tft-ml307` | [README](zhengchen-1.54tft-ml307/README.md) | ST7789 | 无 |
| `zhengchen-1.54tft-wifi` | [README](zhengchen-1.54tft-wifi/README.md) | ST7789 | 无 |

## 构建

在工程根目录激活 ESP-IDF v5.5.2，并指定板型：

```sh
. /path/to/esp-idf/export.sh
test "$(idf.py --version)" = "ESP-IDF v5.5.2"
python3 scripts/build_all.py --board m5stack-core-s3 --language both
```

脚本默认编译中英文版本，构建目录在 `build/`，合并固件在 `releases/`；常规构建会先清理
脚本管理的生成目录，增量构建使用 `--no-clean`。仅英文使用 `--language en-US`，仅中文
使用 `--language zh-CN`。有屏幕的板型自动使用 LVGL；只有 CoreS3 具备视频能力，脚本默认
开启，音频对照固件可使用 `--video off`。

手动构建时，为每块板指定独立目录和 sdkconfig：

```sh
idf.py -B build/m5stack-core-s3 \
  -DMYBOT_BOARD=m5stack-core-s3 \
  -DSDKCONFIG=build/m5stack-core-s3/sdkconfig build
idf.py -B build/m5stack-core-s3 -p PORT flash monitor
```

手动构建使用当前 sdkconfig 的视频选项；上面的命令不自动开启视频。SenseCAP Watcher 的
工厂分区保留与烧录步骤见其专用说明。

## 输入、配网与验证

设备首次启动无 Wi-Fi 凭据时自动配网；长按配网输入会先停止 mybot，再进入配网流程，
联网后重新启动 mybot。各板的配网输入、短按/触摸和音量输入见对应 README。

文档依据代码描述已实现能力，不代表每块硬件均已实机验证。已有验证记录和限制见
[平台移植说明](../../../docs/BOARD_PORTING.zh-CN.md)，构建/烧录完整说明见
[中文项目文档](../../../README.zh-CN.md#构建)或 [English guide](../../../README.md#build)。
