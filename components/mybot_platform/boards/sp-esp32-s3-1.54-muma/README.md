# SP ESP32-S3 1.54 Muma

板型 ID 为 `sp-esp32-s3-1.54-muma`。

- 显示：240×240 ST7789，SPI3 mode 0，40 MHz，共享 LVGL UI。
- 音频：ES8311，I2C0 GPIO15/14，GPIO46 speaker PA。
- 输入：CST816S 兼容触摸和 GPIO0 Boot；触摸使用独立 I2C1 GPIO11/7。
- 存储：16 MB QIO Flash、80 MHz Octal PSRAM、双 OTA。
- 背光：GPIO42，低有效 PWM。

面板 vendor 调优、触摸、音频、电池检测、电源保持和按键仍需真机验证；休眠和关机未实现。

公共构建见[板卡索引](../README.md)，验证状态见[验证矩阵](../../../../docs/VALIDATION.md)。
