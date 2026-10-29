# M5Stack CoreS3

本工程的板型 ID 为 `m5stack-core-s3`，面向带 GC0308 摄像头的 CoreS3。
[官方硬件文档](https://docs.m5stack.com/en/core/CoreS3)。

## 当前配置

| 项目 | 本工程实现 |
| --- | --- |
| SoC | ESP32-S3 |
| Flash | 16 MB，QIO |
| PSRAM | 8 MB，Quad，80 MHz |
| 分区 | [partitions/v2/16m.csv](../../../../partitions/v2/16m.csv)，双 OTA 分区 |
| 控制台 | ESP-IDF 默认 UART0，板级配置未覆盖控制台类型 |
| 音频 | ES7210 采集、AW88298 播放；SDK 边界为 16 kHz 单声道 signed-16 PCM |
| 显示 | 320 × 240 ILI9342，自动使用 LVGL 像素眼镜界面 |
| 输入 | FT6336 触摸：短触开始/结束对话，长按 3 秒进入配网 |
| 摄像头 | GC0308 DVP，320 × 240 YUYV，软件 JPEG，上行最多 1 fps |
| 电源 | AXP2101 与 AW9523，由板级硬件层初始化并控制背光、复位 |

共享 I2C1 为 SDA GPIO12、SCL GPIO11。I2S0 为 MCLK GPIO0、BCLK GPIO34、WS GPIO33、
DIN GPIO14、DOUT GPIO13；GPIO0 用于音频时钟，不作为配网按键。LCD SPI3 为 MOSI GPIO37、
SCLK GPIO36、CS GPIO3、DC GPIO35。

LVGL 对象与 7 KiB UI 任务栈使用 PSRAM，显示条带仍使用 10 KiB 内部 DMA 内存。
板级配置要求开启 `CONFIG_FREERTOS_TASK_CREATE_ALLOW_EXT_MEM`，为摄像头及音频保留内部内存。

无已保存的 Wi-Fi 信息时自动进入配网并显示热点名称、播放提示音。已有网络时联网后启动
SDK；长按触摸会先停止 SDK 再配网。设备音量保存在 NVS，重启后恢复。

CoreS3-SE 不带 GC0308，不能使用视频固件。构建、烧录与验证状态见[板卡索引](../README.md)、
[像素眼镜 UI](../../../../docs/CORES3_UI.zh-CN.md)和[验证矩阵](../../../../docs/VALIDATION.md)。
