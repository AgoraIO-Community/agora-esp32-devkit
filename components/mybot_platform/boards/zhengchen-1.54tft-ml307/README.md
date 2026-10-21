# 征辰 1.54 TFT ML307

本工程的板型 ID 为 `zhengchen-1.54tft-ml307`。硬件参数以本目录的
[board_config.h](board_config.h)、[board.cmake](board.cmake) 和
[sdkconfig.defaults](sdkconfig.defaults) 为准。

## 硬件与已接入功能

| 项目 | 当前实现 |
| --- | --- |
| 主控 | ESP32-S3 |
| Flash | 16 MB，QIO |
| PSRAM | Octal，80 MHz；实际容量以启动日志为准 |
| 分区 | [16 MB 分区表](../../../../partitions/v2/16m.csv)，双 OTA 应用分区及 8 MB assets 分区 |
| 控制台 | 未覆盖 ESP-IDF 默认控制台配置，使用 UART0 |
| 音频 | 独立麦克风/扬声器 I2S，16 kHz 单声道；软件音量及 NVS 持久化 |
| 显示 | ST7789，240×240，SPI3 mode 3，40 MHz；共享 LVGL UI |
| 输入 | Boot 与音量加减按键；无触摸输入 |
| 网络 | Wi-Fi 配网与联网 |
| 视频 | 未接入，不支持开启 `CONFIG_MYBOT_ENABLE_VIDEO` |

ML307 的 RX GPIO11、TX GPIO12 仅在头文件中保留定义；当前 profile 没有 Modem 驱动、
PPP 或 4G 联网实现，网络仍使用 Wi-Fi。

## 引脚与上电行为

| 功能 | 引脚 |
| --- | --- |
| 麦克风 I2S1 RX | WS GPIO4、BCLK GPIO5、DIN GPIO6 |
| 扬声器 I2S0 TX | DOUT GPIO7、BCLK GPIO15、WS GPIO16 |
| Boot / 音量加 / 音量减 | GPIO0 / GPIO10 / GPIO39，低电平有效 |
| LCD | MOSI GPIO41、SCLK GPIO42、CS GPIO21、DC GPIO40、RESET GPIO45 |
| 显示电源 / 背光 | GPIO2 / GPIO20 |

板级初始化先拉高 GPIO2，关闭背光后初始化屏幕、显示启动页并注册按键。GPIO20 使用开关
控制背光，默认点亮；没有 PWM 调光。该板型与征辰 Wi-Fi 板共用
[板级生命周期](../zhengchen-1.54tft-common/board.c)。

SDK 音频边界为 16 kHz、单声道、signed-16 PCM；物理标准 I2S 使用 32-bit mono left slot，
由平台驱动完成数据转换。播放通过公共 PCM 缓冲连续送入 I2S，初始软件音量默认 70，后续
从 NVS 恢复。具体实现见[音频播放说明](../../../../docs/AUDIO_PLAYBACK.zh-CN.md)。

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
python3 scripts/build_all.py --board zhengchen-1.54tft-ml307 --language both
```

合并固件输出为 `releases/zhengchen-1.54tft-ml307-zh-CN.bin` 和对应的 `en-US.bin`。
批量脚本默认清理其专用构建和输出目录，详见[构建说明](../../../../README.zh-CN.md#构建)。

需要使用 `idf.py` 逐步构建、烧录时，使用独立的构建目录和 sdkconfig：

```sh
idf.py -B build/zhengchen-1.54tft-ml307 \
  -DMYBOT_BOARD=zhengchen-1.54tft-ml307 \
  -DSDKCONFIG=build/zhengchen-1.54tft-ml307/sdkconfig build
idf.py -B build/zhengchen-1.54tft-ml307 -p /dev/tty.wchusbserial1430 flash monitor
```

上述串口名为该征辰板在 macOS 上的设备名，其他系统按实际枚举结果选择端口。构建成功
不代表实机验收；音频、屏幕颜色/方向、按键及重复配网应在目标设备上验证，参见
[平台移植说明](../../../../docs/BOARD_PORTING.zh-CN.md)。
