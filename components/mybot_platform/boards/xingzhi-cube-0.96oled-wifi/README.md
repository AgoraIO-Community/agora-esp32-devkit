# 行者 Cube 0.96 OLED Wi-Fi

板型 ID 为 `xingzhi-cube-0.96oled-wifi`。该 profile 当前是 headless 子集，不编入 LVGL。

- 显示：未启用；SSD1306 单色 UI 尚未实现。
- 音频：独立麦克风/扬声器 raw I2S，SDK 边界为 16 kHz 单声道 signed-16 PCM。
- 输入：GPIO0 Boot、GPIO40 音量加、GPIO39 音量减。
- 存储：16 MB QIO Flash profile、80 MHz Octal PSRAM、双 OTA 分区。

初始化、按键极性和音频仍需真机验证。引脚与构建能力以
[board_config.h](board_config.h)、[board.cmake](board.cmake) 和
[sdkconfig.defaults](sdkconfig.defaults) 为准。

公共构建和烧录步骤见[板卡索引](../README.md)，验证状态见
[验证矩阵](../../../../docs/VALIDATION.md)。
