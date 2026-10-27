# Magiclick 2.4

板型 ID 为 `magiclick-2p4`。

- 显示：128×128 NV3023，SPI3 mode 0，40 MHz，共享紧凑 LVGL 布局。
- 音频：ES8311，I2C0 GPIO5/6，GPIO4 speaker PA。
- 输入：GPIO21 主按键、GPIO47 音量加、GPIO0 音量减。
- 存储：16 MB QIO Flash、80 MHz Octal PSRAM、双 OTA。
- 背光：GPIO13 LEDC，默认 60%。

电池管理、LED、休眠和关机未实现；面板、PA、音频和按键仍需真机验证。

公共构建见[板卡索引](../README.md)，验证状态见[验证矩阵](../../../../docs/VALIDATION.md)。
