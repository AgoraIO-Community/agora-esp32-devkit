# Movecall CuiCan ESP32-S3

板型 ID 为 `movecall-cuican-esp32s3`。

- 显示：240×240 GC9A01，SPI3，40 MHz，共享 LVGL UI。
- 音频：ES8311，I2C0 GPIO6/7，GPIO17 speaker PA。
- 输入：GPIO0 Boot；无触摸和音量键。
- 存储：8 MB QIO Flash、80 MHz Octal PSRAM、双 OTA。
- 背光：GPIO16 LEDC，默认 60%。

面板、音频、PA、按键和 8 MB 配置仍需真机验证；LED、电池、休眠和关机未实现。

公共构建见[板卡索引](../README.md)，验证状态见[验证矩阵](../../../../docs/VALIDATION.md)。
