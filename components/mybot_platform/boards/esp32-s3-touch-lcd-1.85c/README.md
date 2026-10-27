# ESP32-S3 Touch LCD 1.85C

板型 ID 为 `esp32-s3-touch-lcd-1.85c`。

- 显示：360×360 ST77916 QSPI，SPI2，共享 LVGL UI。
- 音频：当前实现 V2 的 ES8311 + ES7210 路径。
- 输入：CST816S 兼容触摸和 GPIO0 Boot。
- 扩展：TCA9554 `0x20`，bit0/bit1 用于 LCD/触摸复位。
- 存储：16 MB QIO Flash、80 MHz Octal PSRAM、双 OTA。
- 背光：GPIO5；speaker PA：GPIO15。

V1 的 raw I2S 路径、触摸型号/坐标、面板版本初始化、电源和双 codec 音频仍需真机验证。

公共构建见[板卡索引](../README.md)，验证状态见[验证矩阵](../../../../docs/VALIDATION.md)。
