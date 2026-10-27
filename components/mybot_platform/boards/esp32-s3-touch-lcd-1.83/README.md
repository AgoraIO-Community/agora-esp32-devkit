# ESP32-S3 Touch LCD 1.83

板型 ID 为 `esp32-s3-touch-lcd-1.83`。

- 显示：240×284 ST7789，SPI3 mode 0，24 MHz，共享 LVGL UI。
- 音频：ES8311 与 ES7210，16 kHz mono signed-16 PCM。
- 输入：CST816S 触摸，GPIO0 Boot。
- 电源：AXP2101 `0x34`；GPIO46 speaker PA。
- 存储：16 MB QIO Flash、80 MHz Octal PSRAM、双 OTA。

AXP2101 电源、触摸、面板和双 codec 音频仍需真机验证。PWR41、电池上报、睡眠和关机未接入。

公共构建见[板卡索引](../README.md)，验证状态见[验证矩阵](../../../../docs/VALIDATION.md)。
