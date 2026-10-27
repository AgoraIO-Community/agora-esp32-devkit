# ESP32-S3 LCD 0.85

板型 ID 为 `esp32-s3-lcd-0.85`。

- 显示：128×128 ST7789，SPI3 mode 3，40 MHz，偏移 `(2,1)`。
- 音频：ES8311 与 ES7210，16 kHz mono signed-16 PCM。
- 输入：GPIO5 主按键、GPIO4 音量加、GPIO0 音量减。
- 电源：GPIO2 电池使能；GPIO46 背光；GPIO7 speaker PA。
- 存储：8 MB QIO Flash、80 MHz Octal PSRAM、双 OTA。

当前无触摸。小屏 UI、面板方向、音频、电池和按键仍需真机验证。

公共构建见[板卡索引](../README.md)，验证状态见[验证矩阵](../../../../docs/VALIDATION.md)。
