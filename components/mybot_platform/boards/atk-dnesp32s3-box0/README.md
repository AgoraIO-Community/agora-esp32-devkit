# ATK DNESP32S3 Box0

板型 ID 为 `atk-dnesp32s3-box0`。

- 显示：240×240 ST7789，SPI2，80 MHz，共享 LVGL UI。
- 音频：ES8311，I2C0 GPIO11/12，GPIO21 speaker PA。
- 输入：GPIO4 主按键、GPIO0 音量加、GPIO3 音量减。
- 电源：GPIO2 系统保持、GPIO14 codec 电源、GPIO42 背光。
- 存储：16 MB QIO Flash、80 MHz Octal PSRAM、双 OTA。

电源时序、面板、音频、按键和电池检测仍需真机验证；休眠、深度睡眠和关机未实现。

公共构建见[板卡索引](../README.md)，验证状态见[验证矩阵](../../../../docs/VALIDATION.md)。
