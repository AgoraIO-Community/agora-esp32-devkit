# ESP32-S3 Touch LCD 1.54

板型 ID 为 `esp32-s3-touch-lcd-1.54`。

- 显示：240×240 ST7789，SPI3 mode 3，40 MHz，共享 LVGL UI。
- 音频：ES8311 与 ES7210，16 kHz mono signed-16 PCM。
- 输入：CST816S 触摸、GPIO5 主按键、GPIO4 音量加、GPIO0 音量减。
- 电源：GPIO2 电池/外设使能。
- 存储：16 MB QIO Flash、80 MHz Octal PSRAM、双 OTA。

AXP/电池管理未完整接入；触摸、音频、面板、电源和按键仍需真机验证。睡眠与关机未实现。

公共构建见[板卡索引](../README.md)，验证状态见[验证矩阵](../../../../docs/VALIDATION.md)。
