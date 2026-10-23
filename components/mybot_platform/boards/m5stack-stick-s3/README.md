# M5Stack StickS3

本工程的板型 ID 为 `m5stack-stick-s3`。
[官方硬件文档](https://docs.m5stack.com/zh_CN/core/StickS3)。

## 当前配置

| 项目 | 本工程实现 |
| --- | --- |
| SoC | ESP32-S3 |
| Flash | 8 MB，QIO |
| PSRAM | 8 MB，Octal，80 MHz |
| 分区 | [partitions/v2/8m.csv](../../../../partitions/v2/8m.csv)，双 OTA 分区 |
| 控制台 | ESP-IDF 默认 UART0，板级配置未覆盖控制台类型 |
| 音频 | ES8311；SDK 边界为 16 kHz 单声道 signed-16 PCM，物理 I2S 使用双声道 |
| 显示 | ST7789P3，135 × 240，偏移 (52, 40)，自动使用共享 LVGL 界面 |
| 输入 | GPIO11 主按键，低电平有效；短按开始/结束对话，长按 3 秒进入配网 |
| 电源 | M5PM1，G2 为显示与 codec 供电，G3 控制播放功放 |
| 摄像头 | 当前 profile 未提供视频接口，不支持开启视频 |

共享 I2C0 为 SDA GPIO47、SCL GPIO48，M5PM1 的 7-bit 地址为 `0x6e`，ES8311 为 `0x18`。
I2S0 为 MCLK GPIO18、BCLK GPIO17、WS GPIO15、DIN GPIO16、DOUT GPIO14。
LCD SPI3 为 MOSI GPIO39、SCLK GPIO40、CS GPIO41、DC GPIO45、RESET GPIO21；
背光 GPIO38 由 LEDC 驱动，默认亮度 60%。

固件运行期间保持 G2 电源开启，播放生命周期控制 G3。音量逻辑值为 0–100，保存到 NVS；
当前 ES8311 硬件音量按逻辑值的 60% 映射。无 Wi-Fi 信息时自动进入配网并显示热点名称、
播放提示音；长按主按键会先停止 MyBot 再重新配网。

构建、烧录与验证状态见[板卡索引](../README.md)和[验证矩阵](../../../../docs/VALIDATION.md)。
