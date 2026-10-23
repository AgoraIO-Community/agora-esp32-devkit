# M5Stack AtomEchoS3R

本工程的板型 ID 为 `atom-echos3r`，使用无屏音频 profile。

## 当前配置

| 项目 | 本工程实现 |
| --- | --- |
| SoC | ESP32-S3 |
| Flash | 8 MB，QIO |
| PSRAM | 8 MB，Octal，80 MHz |
| 分区 | [partitions/v2/8m.csv](../../../../partitions/v2/8m.csv)，双 OTA 分区 |
| 控制台 | ESP-IDF 默认 UART0，板级配置未覆盖控制台类型 |
| 音频 | ES8311 采集/播放，GPIO18 控制功放；SDK 使用 16 kHz 单声道 signed-16 PCM |
| 输入 | 当前代码映射 GPIO41 为低电平有效用户按键 |
| 显示 | 无屏幕，不编译 LVGL |
| 摄像头 | 当前 profile 未提供视频接口，不支持开启视频 |

I2C0 使用 SDA GPIO45、SCL GPIO0，ES8311 的 7-bit 地址为 `0x18`。
I2S0 为 MCLK GPIO11、BCLK GPIO17、WS GPIO3、DIN GPIO4、DOUT GPIO48。
GPIO0 已用于 I2C 时钟，不作为配网按键。

当前代码以 GPIO41 短按开始/结束对话、长按 3 秒进入配网；该板不依赖语音唤醒启动对话。
无已保存的 Wi-Fi 信息时自动创建配置热点并播放对应语言的提示音，运行状态和配对码可通过
串口日志查看。音量逻辑值保存到 NVS，初始默认值为 70；ES8311 硬件音量按逻辑值的 60% 映射。

构建、烧录与验证状态见[板卡索引](../README.md)和[验证矩阵](../../../../docs/VALIDATION.md)。
