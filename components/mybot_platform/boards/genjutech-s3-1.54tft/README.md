# GenJuTech S3 1.54 TFT

板型 ID 为 `genjutech-s3-1.54tft`。硬件参数、引脚和能力由本目录的
[`board_config.h`](board_config.h)、[`board.cmake`](board.cmake)、[`board.c`](board.c)
与 [`sdkconfig.defaults`](sdkconfig.defaults) 定义。

| 项目 | 当前实现 |
| --- | --- |
| 存储 | 16 MB QIO Flash profile；80 MHz Octal PSRAM；实际容量待真机确认 |
| 分区 | [`partitions/v2/16m.csv`](../../../../partitions/v2/16m.csv)，双 OTA |
| 音频 | ES8311，16 kHz 单声道 PCM；GPIO21 扬声器功放 |
| 显示 | 240×240 ST7789，SPI3 mode 3，80 MHz，颜色反转，共享 LVGL |
| 输入 | GPIO0 Boot、GPIO42 音量加、GPIO41 音量减，均低电平有效 |
| 网络 / 视频 | Wi-Fi；未接入摄像头，不支持视频 |

| 信号 | GPIO |
| --- | --- |
| I2S MCLK / BCLK / WS / DIN / DOUT | 14 / 13 / 11 / 12 / 10 |
| ES8311 I2C SDA / SCL | 9 / 8 |
| LCD MOSI / SCLK / DC / CS / RESET | 3 / 4 / 5 / 6 / 7 |
| LCD 背光 | 2，LEDC 5 kHz、13-bit，默认 60% |

屏幕、音频和输入在板级初始化后由进程持有。该板的扬声器使能与显示 I/O 存在板级耦合，
因此首次播放开启 GPIO21 后保持有效，直到板级硬件拆除；停止播放仅让 I2S 流空闲。

## 操作

- 短按 Boot：开始或结束对话。
- 短按音量加/减：调节并持久化软件音量。
- 长按 Boot 3 秒：停止 MyBot 后进入 Wi-Fi 配网。

当前未实现电量/充电、LED、触摸、摄像头、休眠、关机或低功耗行为。构建与真机状态见
[验证矩阵](../../../../docs/VALIDATION.md)。
