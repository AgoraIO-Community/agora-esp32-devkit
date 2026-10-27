# Quandong S3 Dev

板型 ID 为 `quandong-s3-dev`。

- 显示：320×240 ILI9341，SPI2，共享 LVGL UI。
- 音频：ES8311，16 kHz 单声道 signed-16 PCM。
- 输入：GPIO0 Boot；无触摸和音量键。
- 扩展：PCA9557 `0x19`；GPIO1 为低有效 PA 使能。
- 存储：16 MB QIO Flash profile、80 MHz Octal PSRAM、双 OTA 分区。

PCA9557 输出映射、ILI9341 厂商初始化、PA 极性和时序、音频及背光仍需真机验证。

引脚见 [board_config.h](board_config.h)；公共构建见[板卡索引](../README.md)，
验证状态见[验证矩阵](../../../../docs/VALIDATION.md)。
