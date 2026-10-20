# Board documentation / 板卡说明

All 13 supported board profiles have documentation for the hardware configuration and firmware
features implemented by `agora-esp32-devkit`. See the [English build guide](../../../README.md#build)
or [中文构建说明](../../../README.zh-CN.md#构建) for environment setup and build options.

各板型说明按本仓库的 `board.cmake`、`board_config.h` 和 `sdkconfig.defaults` 整理。
`MYBOT_BOARD` 是手动构建时的板型选择入口；自动脚本在工程根目录运行：

```sh
python3 scripts/build_all.py --board <board-id> --language both
```

默认生成目录为 `build/build-<板型>-<语言>[-video]/`，合并产物位于 `releases/`。
CoreS3 默认启用视频并带 `-video` 后缀，其余板型关闭视频。Watcher 必须保留工厂 NVS，
使用其说明中的分段烧录步骤。脚本的目录清理和归属检查见根构建指南。

| Board profile | 文档 | 当前配置说明 |
| --- | --- | --- |
| `atom-echos3r` | [README](atom-echos3r/README.md) | ES8311、GPIO41 按键、无屏 |
| `bread-compact-wifi-lcd` | [README](bread-compact-wifi-lcd/README.md) | ST7789、独立 I2S、Boot 按键 |
| `esp-vocat` | [README](esp-vocat/README.md) | ES7210/ES8311、ST77916 与触摸 |
| `esp32-s3-touch-amoled-1.75` | [README](esp32-s3-touch-amoled-1.75/README.md) | CO5300 AMOLED 与触摸 |
| `esp32-s3-touch-amoled-1.75c` | [README](esp32-s3-touch-amoled-1.75c/README.md) | 独立 1.75C profile |
| `esp32-s3-touch-amoled-2.16` | [README](esp32-s3-touch-amoled-2.16/README.md) | 480×480 CO5300 AMOLED |
| `m5stack-core-s3` | [README](m5stack-core-s3/README.md) | 像素眼镜 UI，默认摄像头视频 |
| `m5stack-stick-s3` | [README](m5stack-stick-s3/README.md) | ST7789、ES8311 与主按键 |
| `respeaker-flex-xvf3800-circular4-xiao` | [README](respeaker-flex-xvf3800-circular4-xiao/README.md) | Circular-4、XVF3800 与 AIC3104，无屏 |
| `sensecap-watcher` | [中文](sensecap-watcher/README.md) / [English](sensecap-watcher/README_en.md) | SPD2010、编码器与工厂数据保护 |
| `xingzhi-cube-1.54tft-wifi` | [README](xingzhi-cube-1.54tft-wifi/README.md) | ST7789、独立 I2S 与音量按键 |
| `zhengchen-1.54tft-ml307` | [README](zhengchen-1.54tft-ml307/README.md) | ST7789、独立 I2S，当前使用 Wi-Fi |
| `zhengchen-1.54tft-wifi` | [README](zhengchen-1.54tft-wifi/README.md) | ST7789、独立 I2S 与音量按键 |

硬件参考资料及文档来源见
[VENDORED_SOURCES.md](../../../VENDORED_SOURCES.md#board-reference-documents)，许可归属见
[THIRD_PARTY_NOTICES.md](../../../THIRD_PARTY_NOTICES.md)。构建结果与实机验证状态分别记录在
[平台移植说明](../../../docs/BOARD_PORTING.zh-CN.md)中。
