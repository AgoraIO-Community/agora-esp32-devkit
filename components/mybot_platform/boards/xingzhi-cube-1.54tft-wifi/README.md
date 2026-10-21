# 行者 Cube 1.54 TFT Wi-Fi

本工程的板型 ID 为 `xingzhi-cube-1.54tft-wifi`。接线和能力由本目录的
[board_config.h](board_config.h)、[board.cmake](board.cmake)、
[board.c](board.c) 和 [sdkconfig.defaults](sdkconfig.defaults) 定义。

## 硬件与已接入功能

| 项目 | 当前实现 |
| --- | --- |
| 主控 | ESP32-S3 |
| Flash | 16 MB QIO profile；实际容量需在目标设备核对 |
| PSRAM | Octal，80 MHz；实际容量需在目标设备核对 |
| 分区 | [16 MB 分区表](../../../../partitions/v2/16m.csv)，双 OTA 应用分区及 8 MB assets 分区 |
| 控制台 | 未覆盖 ESP-IDF 默认控制台配置，使用 UART0 |
| 音频 | 独立麦克风/扬声器 I2S，16 kHz 单声道；软件音量及 NVS 持久化 |
| 显示 | ST7789，240×240，SPI3 mode 3，40 MHz；共享 LVGL UI |
| 输入 | Boot 与音量加减按键；无触摸输入 |
| 电源保持 | GPIO21，整个进程运行期间保持高电平 |
| 网络 / 视频 | Wi-Fi；未接入摄像头，不支持开启 `CONFIG_MYBOT_ENABLE_VIDEO` |

## 引脚与上电行为

| 功能 | 引脚 |
| --- | --- |
| 麦克风 I2S1 RX | WS GPIO4、BCLK GPIO5、DIN GPIO6 |
| 扬声器 I2S0 TX | DOUT GPIO7、BCLK GPIO15、WS GPIO16 |
| Boot / 音量加 / 音量减 | GPIO0 / GPIO40 / GPIO39，低电平有效 |
| LCD | MOSI GPIO10、SCLK GPIO9、CS GPIO14、DC GPIO8、RESET GPIO18 |
| 背光 | GPIO13，LEDC 5 kHz、13-bit PWM，默认 60% |

板级初始化先解除 GPIO21 的 RTC hold、恢复普通 GPIO 并拉高以保持系统供电，再初始化
背光、屏幕和常驻按键。面板使用 RGB 顺序、零偏移、不镜像并启用颜色反转。

SDK 音频边界为 16 kHz、单声道、signed-16 PCM；物理标准 I2S 使用 32-bit mono left slot，
由公共 raw I2S 驱动完成转换。播放通过公共 PCM 缓冲连续送入 I2S，初始软件音量默认 70，
后续从 NVS 恢复。当前实现未接入电池/充电监测、关机、休眠或低功耗。

## 配网与操作

- 短按 Boot：就绪时开始对话，对话中结束对话。
- 长按 Boot 3 秒：请求配网；应用先停止 mybot，再进入配网模式。
- 短按音量加减键：调节扬声器音量并持久化。

没有 Wi-Fi 凭据时自动进入配网。LCD 显示实际热点名称 `mybot-xxxx`，长文字自动滚动；
连接该热点后访问 `http://192.168.4.1` 配置 Wi-Fi。进入配网时播放对应语言提示音，取得
可用 IP 后才启动 mybot。

## 构建与烧录

在工程根目录激活 ESP-IDF v5.5.2 后构建中英文固件：

```sh
python3 scripts/build_all.py --board xingzhi-cube-1.54tft-wifi --language both
```

合并固件输出为 `releases/xingzhi-cube-1.54tft-wifi-zh-CN.bin` 和对应的 `en-US.bin`。
批量脚本默认清理其专用构建和输出目录，详见[构建说明](../../../../README.zh-CN.md#构建)。

使用独立构建目录和 sdkconfig 的手动构建、烧录示例，将 `PORT` 替换为本机串口设备：

```sh
idf.py -B build/xingzhi-cube-1.54tft-wifi \
  -DMYBOT_BOARD=xingzhi-cube-1.54tft-wifi \
  -DSDKCONFIG=build/xingzhi-cube-1.54tft-wifi/sdkconfig build
idf.py -B build/xingzhi-cube-1.54tft-wifi -p PORT flash monitor
```

构建成功不代表实机验收。实际 Flash/PSRAM 容量、GPIO21 供电保持、PWM 背光、显示方向、
音频速度/音调和同时采集播放均需在目标硬件核对；验收范围见
[平台移植说明](../../../../docs/BOARD_PORTING.zh-CN.md)。
