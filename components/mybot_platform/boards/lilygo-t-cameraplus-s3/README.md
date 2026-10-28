# LilyGO T-CameraPlus-S3 V1.0/V1.1

板型 ID 为 `lilygo-t-cameraplus-s3`。

- 显示：240×240 ST7789，SPI3 mode 0，60 MHz，共享 LVGL UI。
- 音频：MSM261 标准 I2S 采集和 MAX98357A 播放，16 kHz 单声道 signed-16 PCM。
- 触摸：CST816D，I2C GPIO1/GPIO2，中断 GPIO47，复位 GPIO48。
- 输入：GPIO0 Boot。
- 存储：16 MB QIO Flash、80 MHz Octal PSRAM、双 OTA。

V1.2 使用不同引脚和 PDM 麦克风，不能烧录本 profile。摄像头、SY6970 电池管理、KEY1、SD 卡、
红外和休眠未实现。面板、触摸和音频仍需真机验证。

V1.2 使用 `lilygo-t-cameraplus-s3-v1.2`。
