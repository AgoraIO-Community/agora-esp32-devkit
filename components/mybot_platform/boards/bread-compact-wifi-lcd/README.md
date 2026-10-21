# Bread Compact Wi-Fi LCD

本工程的板型 ID 为 `bread-compact-wifi-lcd`，对应 ESP32-S3 N16R8 与 240×320 ST7789 的
固定接线组合。接线和能力由本目录的 [board_config.h](board_config.h)、
[board.cmake](board.cmake)、[board.c](board.c) 和
[sdkconfig.defaults](sdkconfig.defaults) 定义。

## 硬件与已接入功能

| 项目 | 当前实现 |
| --- | --- |
| 主控 | ESP32-S3 N16R8 profile |
| Flash | 16 MB，QIO |
| PSRAM | 8 MB，Octal，80 MHz |
| 分区 | [16 MB 分区表](../../../../partitions/v2/16m.csv)，双 OTA 应用分区及 8 MB assets 分区 |
| 控制台 | 未覆盖 ESP-IDF 默认控制台配置，使用 UART0 |
| 音频 | 独立麦克风/扬声器 I2S，16 kHz 单声道；软件音量及 NVS 持久化 |
| 显示 | ST7789，240×320，SPI3 mode 0，40 MHz；共享 LVGL UI |
| 输入 | 单个 Boot 按键；无独立音量键、无触摸输入 |
| 网络 / 视频 | Wi-Fi；未接入摄像头，不支持开启 `CONFIG_MYBOT_ENABLE_VIDEO` |

## 引脚与上电行为

| 功能 | 引脚 |
| --- | --- |
| 麦克风 I2S1 RX | WS GPIO4、BCLK GPIO5、DIN GPIO6 |
| 扬声器 I2S0 TX | DOUT GPIO7、BCLK GPIO15、WS GPIO16 |
| Boot | GPIO0，低电平有效 |
| LCD | MOSI GPIO47、SCLK GPIO21、CS GPIO41、DC GPIO40、RESET GPIO45 |
| 背光 | GPIO42，LEDC 5 kHz、13-bit PWM，默认 60% |

板级初始化先设置 PWM 背光为关闭，再初始化面板、显示启动页并注册常驻按键。面板使用
RGB 顺序、零偏移、不镜像并启用颜色反转。本 profile 没有 GPIO 电源保持，也不驱动
GPIO48 LED 或 GPIO18 灯输出；其他尺寸、不同接线或共享 I2S 时钟的硬件需使用独立 profile。

SDK 音频边界为 16 kHz、单声道、signed-16 PCM；物理标准 I2S 使用 32-bit mono left slot，
由公共 raw I2S 驱动完成转换。播放通过公共 PCM 缓冲连续送入 I2S，初始软件音量默认 70，
后续从 NVS 恢复；本板没有实体音量加减键。

## 配网与操作

- 短按 Boot：就绪时开始对话，对话中结束对话。
- 长按 Boot 3 秒：请求配网；应用先停止 mybot，再进入配网模式。按键在 mybot 停止期间
  仍可触发配网请求。

没有 Wi-Fi 凭据时自动进入配网。LCD 显示实际热点名称 `mybot-xxxx`，长文字自动滚动；
连接该热点后访问 `http://192.168.4.1` 配置 Wi-Fi。进入配网时播放对应语言提示音，取得
可用 IP 后才启动 mybot。

## 构建与烧录

在工程根目录激活 ESP-IDF v5.5.2 后构建中英文固件：

```sh
python3 scripts/build_all.py --board bread-compact-wifi-lcd --language both
```

合并固件输出为 `releases/bread-compact-wifi-lcd-zh-CN.bin` 和对应的 `en-US.bin`。
批量脚本默认清理其专用构建和输出目录，详见[构建说明](../../../../README.zh-CN.md#构建)。

使用独立构建目录和 sdkconfig 的手动构建、烧录示例，将 `PORT` 替换为本机串口设备：

```sh
idf.py -B build/bread-compact-wifi-lcd \
  -DMYBOT_BOARD=bread-compact-wifi-lcd \
  -DSDKCONFIG=build/bread-compact-wifi-lcd/sdkconfig build
idf.py -B build/bread-compact-wifi-lcd -p PORT flash monitor
```

该固定 profile 的中英文构建支持已在 ESP-IDF v5.5.2 验证，真机验证尚未完成。目标设备仍需
核对模组与检测容量、屏幕颜色/方向、PWM 背光、音频速度/音调、全双工交互及重复配网。
验收范围见[平台移植说明](../../../../docs/BOARD_PORTING.zh-CN.md)。
