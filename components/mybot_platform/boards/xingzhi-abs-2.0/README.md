# 行者 ABS 2.0

板型 ID 为 `xingzhi-abs-2.0`。当前只实现 Wi-Fi 子集，不包含蜂窝网络。

- 显示：240×240 ST7789，SPI3 mode 3，共享 LVGL UI。
- 音频：ES8311，16 kHz 单声道 signed-16 PCM；I2C0 GPIO41/42。
- 输入：GPIO0 Boot、GPIO45 音量加、GPIO2 音量减。
- 扩展：PCA9557 `0x19`；GPIO47 进程级电源保持。
- 存储：16 MB QIO Flash profile、80 MHz Octal PSRAM、双 OTA 分区。

SD 卡、振动、蜂窝、电池/充电、扩展电源路径、休眠和关机未实现。PCA9557、PA、面板、触摸
能力与上电顺序仍需真机验证。

引脚以 [board_config.h](board_config.h) 为准；公共构建见[板卡索引](../README.md)，
验证状态见[验证矩阵](../../../../docs/VALIDATION.md)。
