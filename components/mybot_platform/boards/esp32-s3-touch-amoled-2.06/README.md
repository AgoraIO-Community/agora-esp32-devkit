# ESP32-S3 Touch AMOLED 2.06

板型 ID 为 `esp32-s3-touch-amoled-2.06`。

- 显示：410×502 SH8601 QSPI，X 方向 panel gap 为 22 像素。
- 音频：ES8311 输出与 ES7210 采集，16 kHz 单声道 signed-16 PCM。
- 输入：FT5x06 触摸和 GPIO0 Boot。
- 电源：AXP2101 `0x34`。
- 存储：16 MB QIO Flash profile、80 MHz Octal PSRAM、双 OTA 分区。

面板时序、偏移、颜色、触摸映射、AXP2101 电源和双 codec 音频仍需真机验证。睡眠、关机、
电池上报和视频未实现。

引脚见 [board_config.h](board_config.h)；公共构建见[板卡索引](../README.md)，
验证状态见[验证矩阵](../../../../docs/VALIDATION.md)。
