# Waveshare ESP32-S3-Touch-AMOLED-1.75C

本目录是 agora-esp32-devkit 的 `esp32-s3-touch-amoled-1.75c` 板级 profile。1.75C 与非 C 版本
的 MCLK、显示和触摸 reset 不同，需选择本板专用固件，不能按屏幕尺寸混用。

## 硬件与当前 profile

| 项目 | 当前配置 |
| --- | --- |
| 主控与存储 | ESP32-S3、保守的 16 MB QIO Flash profile、80 MHz 8 MB Octal PSRAM |
| 分区 | 根目录 `partitions/v2/16m.csv` |
| 音频 | ES7210 采集、ES8311 播放，16 kHz 单声道 signed 16-bit PCM |
| 公共 I2C0 | SDA GPIO15、SCL GPIO14；AXP2101 `0x34`、ES8311 `0x18`、ES7210 `0x40`、CST9217 `0x5a` |
| I2S | MCLK GPIO16、BCLK GPIO9、WS GPIO45、DIN GPIO10、DOUT GPIO8；PA GPIO46 |
| 显示 | 466 × 466 CO5300 QSPI AMOLED，显示偏移 (6, 0)，共享 LVGL UI |
| 显示 reset / 触摸 reset | GPIO1 / GPIO2 |
| 输入 | CST9217 中断 GPIO11、Boot GPIO0 |
| TCA9554 | 不探测且不依赖 |

产品容量描述存在差异；本工程按 16 MB Flash、8 MB PSRAM 使用保守配置，实际物理容量仍需
通过启动日志核对。确认实际容量之前不要扩大分区。完整定义见
[board_config.h](board_config.h)、[board.cmake](board.cmake) 与
[sdkconfig.defaults](sdkconfig.defaults)。

## 构建

在工程根目录激活 ESP-IDF v5.5.2 后构建中英文固件：

```sh
. /path/to/esp-idf/export.sh
test "$(idf.py --version)" = "ESP-IDF v5.5.2"
python3 scripts/build_all.py --board esp32-s3-touch-amoled-1.75c --language both
```

中间构建目录为 `build/build-esp32-s3-touch-amoled-1.75c-<语言>/`，默认生成根目录 `releases/esp32-s3-touch-amoled-1.75c-zh-CN.bin` 与
`releases/esp32-s3-touch-amoled-1.75c-en-US.bin`。构建器默认清理所选构建/输出根目录，详情与烧录入口见
[根目录构建说明](../../../../README.zh-CN.md#构建)。本板不支持视频构建。

## 使用与验证边界

短触屏幕或短按 Boot 开始/结束对话，长触或长按 3 秒进入配网。采集只提供主麦 slot，保留
Cloud AEC；未向 SDK 提供播放参考通道或本地 AEC。当前未支持 RTC、IMU、TF 卡、TCA9554、
电池状态、自动休眠与关机手势。

本 profile 尚未完成真机验证；电源时序、显示方向和颜色、触摸、容量及 16 kHz 双向音频
均需在实物上确认，不能以构建成功代替。

板级集成约定见 [板级移植](../../../../docs/BOARD_PORTING.zh-CN.md)，
相关上游许可见 [MIT 许可文本](../../assets/LICENSE.xiaozhi-esp32)。
