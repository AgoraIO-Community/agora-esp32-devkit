# Bread Compact Wi-Fi

板型 ID 为 `bread-compact-wifi`。当前为 headless profile。

- 音频：独立麦克风/扬声器 I2S，16 kHz mono signed-16 PCM。
- 输入：GPIO0 Boot、GPIO40 音量加、GPIO39 音量减。
- 存储：16 MB QIO Flash、80 MHz Octal PSRAM、双 OTA。

SSD1306/SH1106 单色屏 UI、GPIO47 触摸按键、GPIO18 灯和 GPIO48 LED 未接入。音频、
按键和实际 Flash/PSRAM 仍需真机验证。

公共构建见[板卡索引](../README.md)，验证状态见[验证矩阵](../../../../docs/VALIDATION.md)。
