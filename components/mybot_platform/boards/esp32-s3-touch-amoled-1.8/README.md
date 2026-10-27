# ESP32-S3 Touch AMOLED 1.8

板型 ID 为 `esp32-s3-touch-amoled-1.8`。

- 显示：368×448 SH8601 QSPI，共享 LVGL UI。
- 音频：ES8311，16 kHz 单声道 signed-16 PCM。
- 输入：FT5x06 触摸和 GPIO0 Boot。
- 电源/扩展：AXP2101 `0x34`、TCA9554 `0x20`，用于电源轨和 LCD/触摸复位。
- 存储：16 MB QIO Flash profile、80 MHz Octal PSRAM、双 OTA 分区。

AXP2101/TCA9554 上电顺序、SH8601 时序、触摸映射、音频和 PA 仍需真机验证。睡眠、关机、
电池上报和视频未实现。

引脚见 [board_config.h](board_config.h)；公共构建见[板卡索引](../README.md)，
验证状态见[验证矩阵](../../../../docs/VALIDATION.md)。
