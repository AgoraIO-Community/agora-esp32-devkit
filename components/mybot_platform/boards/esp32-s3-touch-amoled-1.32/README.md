# ESP32-S3 Touch AMOLED 1.32

板型 ID 为 `esp32-s3-touch-amoled-1.32`。

- 显示：466×466 SH8601 QSPI，共享 LVGL UI。
- 音频：ES8311，16 kHz 单声道 signed-16 PCM。
- 输入：GPIO0 Boot；当前未实现触摸。
- 电源：GPIO18 电源使能。
- 存储：8 MB QIO Flash profile、80 MHz Octal PSRAM、双 OTA 分区。

面板偏移、颜色、方向、电源轨、音频和按键仍需真机验证；睡眠、关机、电池上报和视频未实现。

引脚见 [board_config.h](board_config.h)；公共构建见[板卡索引](../README.md)，
验证状态见[验证矩阵](../../../../docs/VALIDATION.md)。
