# ESP32-S3 Korvo2 V3

板型 ID 为 `esp32s3-korvo2-v3`。

- 显示：320×240 ILI9341，SPI3，TCA9554 控制 LCD CS 与复位。
- 音频：ES8311 输出与 ES7210 采集，16 kHz 单声道 signed-16 PCM。
- 输入：GPIO5 Boot；ADC 按键尚未实现。
- 存储：16 MB QIO Flash profile、80 MHz Octal PSRAM、双 OTA 分区。
- 扩展：TCA9554 `0x20`；GPIO48 扬声器 PA。

ADC 按键、摄像头、电池上报、休眠和板级电源管理未实现。TCA9554 复位/CS 时序、ILI9341、
双 codec 路由和音频仍需真机验证。

引脚见 [board_config.h](board_config.h)；公共构建见[板卡索引](../README.md)，
验证状态见[验证矩阵](../../../../docs/VALIDATION.md)。
