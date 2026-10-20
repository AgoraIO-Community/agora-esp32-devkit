# 征辰 1.54 TFT Wi-Fi

本说明对应 `agora-esp32-devkit` 的 `zhengchen-1.54tft-wifi` profile。
硬件配置以本目录的 [board_config.h](board_config.h)、[board.cmake](board.cmake) 和
[sdkconfig.defaults](sdkconfig.defaults) 为准。

## 当前配置

| 项目 | 配置 |
| --- | --- |
| MCU | ESP32-S3 |
| 存储 | 16 MB QIO Flash profile；Octal PSRAM，80 MHz |
| 显示 | 240×240 ST7789，SPI mode 3；LVGL 状态界面 |
| 音频 | 麦克风与扬声器使用独立 I2S，16 kHz 单声道 PCM |
| 麦克风 WS/BCLK/DIN | GPIO4/5/6 |
| 扬声器 DOUT/BCLK/WS | GPIO7/15/16 |
| 输入 | Boot GPIO0、音量加 GPIO10、音量减 GPIO39 |
| 分区 | [`partitions/v2/16m.csv`](../../../../partitions/v2/16m.csv) |

短按 Boot 开始或结束对话，长按 3 秒进入配网；音量按键调节并持久化扬声器音量。
当前固件通过 Wi-Fi 联网；实际 PSRAM 容量仍需在目标硬件上核对。

## 构建与烧录

在工程根目录加载 ESP-IDF v5.5.2 环境后执行：

```sh
python3 scripts/build_all.py --board zhengchen-1.54tft-wifi --language both
```

脚本通过 `MYBOT_BOARD` 选择本 profile，默认生成
`releases/zhengchen-1.54tft-wifi-zh-CN.bin` 和
`releases/zhengchen-1.54tft-wifi-en-US.bin`。分语言构建目录位于
`build/build-zhengchen-1.54tft-wifi-<语言>/`。

例如烧录中文生成工程并查看日志，将串口替换为实际端口：

```sh
idf.py -B build/build-zhengchen-1.54tft-wifi-zh-CN -p /dev/ttyUSB0 flash monitor
```

环境、输出目录清理规则及其他构建选项见[项目构建指南](../../../../README.zh-CN.md#构建)。
充电状态、电池 ADC、温度监测、自动休眠和低功耗尚未接入；构建成功不能替代实机验证。
验证状态见[平台移植说明](../../../../docs/BOARD_PORTING.zh-CN.md)，第三方归属见
[许可说明](../../../../THIRD_PARTY_NOTICES.md)。
