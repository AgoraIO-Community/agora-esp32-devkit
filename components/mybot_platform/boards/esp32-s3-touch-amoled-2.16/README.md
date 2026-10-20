# Waveshare ESP32-S3-Touch-AMOLED-2.16

本目录是 agora-esp32-devkit 的 `esp32-s3-touch-amoled-2.16` 板级 profile，面向
[Waveshare ESP32-S3-Touch-AMOLED-2.16](https://www.waveshare.com/esp32-s3-touch-amoled-2.16.htm)。
本板使用独立的 480 像素面板配置，与 1.75/1.75C 固件不能混用。

## 硬件与当前 profile

| 项目 | 当前配置 |
| --- | --- |
| 主控与存储 | ESP32-S3、保守的 16 MB QIO Flash profile、80 MHz Octal PSRAM；实际容量待核对 |
| 分区 | 根目录 `partitions/v2/16m.csv` |
| 音频 | ES7210 采集、ES8311 播放，16 kHz 单声道 signed 16-bit PCM |
| 公共 I2C0 | SDA GPIO15、SCL GPIO14；AXP2101 `0x34`、ES8311 `0x18`、ES7210 `0x40`、CST9217 `0x5a` |
| I2S | MCLK GPIO42、BCLK GPIO9、WS GPIO45、DIN GPIO10、DOUT GPIO8；PA GPIO46 |
| 显示 | 480 × 480 CO5300 AMOLED，40 MHz QSPI，显示偏移 (0, 0)，共享 LVGL UI |
| 显示 reset / 触摸 reset | GPIO39 / GPIO40 |
| 输入 | CST9217 中断 GPIO11、Boot GPIO0 |
| TCA9554 | 在 `0x20` 可选探测，实物是否搭载待核对 |

面板使用 RGB565 局部 DMA 刷新，并保持刷新区域起止像素边界为偶数。Flash 与 PSRAM 的实际
物理容量必须在发布设备上确认。完整配置见 [board_config.h](board_config.h)、
[board.cmake](board.cmake) 与 [sdkconfig.defaults](sdkconfig.defaults)。

## 构建

在工程根目录激活 ESP-IDF v5.5.2 后构建中英文固件：

```sh
. /path/to/esp-idf/export.sh
test "$(idf.py --version)" = "ESP-IDF v5.5.2"
python3 scripts/build_all.py --board esp32-s3-touch-amoled-2.16 --language both
```

中间构建目录为 `build/build-esp32-s3-touch-amoled-2.16-<语言>/`，默认生成根目录 `releases/esp32-s3-touch-amoled-2.16-zh-CN.bin` 与
`releases/esp32-s3-touch-amoled-2.16-en-US.bin`。构建器默认清理所选构建/输出根目录，详情与烧录入口见
[根目录构建说明](../../../../README.zh-CN.md#构建)。本板不支持视频构建。

## 使用与验证边界

短触屏幕或短按 Boot 开始/结束对话，长触或长按 3 秒进入配网。采集只提供主麦 slot，保留
Cloud AEC；未向 SDK 提供播放参考通道或本地 AEC。RTC、IMU、TF 卡、电量、关机与低功耗
尚未接入。

本 profile 已有中英文 ESP-IDF v5.5.2 构建验证，尚未完成真机验证。电源时序、显示方向和
颜色、CST9217 输入、TCA9554 搭载情况及 16 kHz 双向音频仍需实物确认。

板级集成约定见 [板级移植](../../../../docs/BOARD_PORTING.zh-CN.md)，
相关上游许可见 [MIT 许可文本](../../assets/LICENSE.xiaozhi-esp32)。
