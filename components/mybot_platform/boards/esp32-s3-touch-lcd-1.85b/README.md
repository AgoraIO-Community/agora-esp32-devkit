# ESP32-S3 Touch LCD 1.85B

板型 ID 为 `esp32-s3-touch-lcd-1.85b`。

- 显示：360×360 ST77916 QSPI，SPI2，共享 LVGL UI。
- 音频：ES8311 与 ES7210，16 kHz mono signed-16 PCM。
- 输入：CST816S 兼容触摸和 GPIO0 Boot。
- 存储：16 MB QIO Flash、80 MHz Octal PSRAM、双 OTA。
- 面板复位：GPIO3；背光：GPIO5。

该板存在面板版本差异，当前使用 ST77916 默认初始化，厂商版本探测和专用初始化尚未接入。
触摸型号/坐标、面板时序/颜色、音频和电源仍需真机验证。

公共构建见[板卡索引](../README.md)，验证状态见[验证矩阵](../../../../docs/VALIDATION.md)。
