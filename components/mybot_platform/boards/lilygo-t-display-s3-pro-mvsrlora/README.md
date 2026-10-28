# LilyGO T-Display-S3-Pro MVSRLoRa

板型 ID 为 `lilygo-t-display-s3-pro-mvsrlora`。

- 显示：222×480 ST7796，SPI3 mode 0，40 MHz，X 偏移 49，共享 LVGL UI。
- 音频：MP34DT05TR PDM 采集和 MAX98357A 播放，16 kHz 单声道 signed-16 PCM。
- 触摸：CST226SE，I2C0 GPIO5/GPIO6，地址 `0x5A`，复位 GPIO13。
- 输入：GPIO0 Boot；GPIO41 控制扬声器使能。
- 电源：SY6970 `0x6A` 关闭 ILIM、启用 ADC 并关闭看门狗。
- 存储：16 MB QIO Flash、80 MHz Octal PSRAM、双 OTA。
- 背光：GPIO48，默认 60%。

SD 卡、LoRa、振动、RTC、IMU、电池和休眠未实现。ST7796、CST226SE、SY6970、PDM 麦克风、
扬声器使能和面板方向仍需真机验证。

公共构建见[板卡索引](../README.md)，验证状态见[验证矩阵](../../../../docs/VALIDATION.md)。
