# LilyGO T-CameraPlus-S3 V1.2

板型 ID 为 `lilygo-t-cameraplus-s3-v1.2`。

- 显示：240×240 ST7789，SPI3 mode 0，60 MHz，共享 LVGL UI。
- 音频：MP34DT05TR PDM 采集，MAX98357A 播放，16 kHz 单声道 signed-16 PCM。
- 触摸：CST816D，I2C GPIO33/GPIO37，中断 GPIO47，无硬件复位。
- 输入：GPIO0 Boot。
- 存储：16 MB QIO Flash、80 MHz Octal PSRAM、双 OTA。

本 profile 仅用于 V1.2，不能与 V1.0/V1.1 交叉烧录。摄像头、SY6970 电池管理、KEY1、SD 卡、
红外和休眠未实现。面板、触摸、PDM 麦克风和扬声器使能仍需真机验证。

公共构建见[板卡索引](../README.md)，验证状态见[验证矩阵](../../../../docs/VALIDATION.md)。
