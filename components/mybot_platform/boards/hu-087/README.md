# HU-087

板型 ID 为 `hu-087`。当前为 headless profile。

- 音频：独立麦克风/扬声器 I2S，16 kHz mono signed-16 PCM。
- 功放控制：GPIO17，随播放启停。
- 输入：GPIO18 触摸/按键。
- 存储：16 MB QIO Flash、80 MHz Octal PSRAM、双 OTA。

SSD1306 单色屏 UI 未接入；长按被映射为配网请求，原固件的长按音量行为不再保留。
音频、功放、输入和实际 Flash/PSRAM 仍需真机验证。

公共构建见[板卡索引](../README.md)，验证状态见[验证矩阵](../../../../docs/VALIDATION.md)。
