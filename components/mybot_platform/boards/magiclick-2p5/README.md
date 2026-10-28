# Magiclick 2.5

板型 ID 为 `magiclick-2p5`。当前实现仅使用 Wi-Fi 通路。

- 显示：128×128，支持 GC9107 与 ST7735 两种 PCB 版本，共享紧凑 LVGL UI。
- 音频：ES8311，I2C0 GPIO5/6，GPIO4 speaker PA，16 kHz 单声道 signed-16 PCM。
- 输入：GPIO21 Boot、GPIO47 音量加、GPIO0 音量减。
- 存储：16 MB QIO Flash、80 MHz Octal PSRAM、双 OTA。
- 背光：GPIO13 LEDC，默认 60%。

ML307/4G、电池、充电、LED、休眠和关机未实现。面板版本由 GPIO3 ADC 选择，显示方向、
音频、PA、按键和电源仍需真机验证。

公共构建见[板卡索引](../README.md)，验证状态见[验证矩阵](../../../../docs/VALIDATION.md)。
