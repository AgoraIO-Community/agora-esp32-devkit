# Freenove ESP32-S3 Display 2.8 LCD

板型 ID 为 `freenove-esp32s3-display-2.8-lcd`。

- 显示：320×240 ILI9341，SPI3，20 MHz，共享 LVGL UI。
- 音频：ES8311，I2C0 GPIO16/15，GPIO1 speaker PA。
- 输入：GPIO0 Boot；无触摸和音量键。
- 存储：16 MB QIO Flash、80 MHz Octal PSRAM、双 OTA。
- 背光：GPIO45 LEDC，默认 60%。

面板、音频、PA 和按键仍需真机验证。摄像头、LED、电池、休眠和关机未实现。

公共构建见[板卡索引](../README.md)，验证状态见[验证矩阵](../../../../docs/VALIDATION.md)。
