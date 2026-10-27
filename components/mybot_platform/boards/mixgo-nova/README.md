# MIXGO Nova

板型 ID 为 `mixgo-nova`。

- 显示：128×160 ST7789，SPI3，共享 LVGL 紧凑布局。
- 音频：ES8374 `0x20`，16 kHz 单声道 signed-16 PCM。
- 输入：GPIO0 Boot；无触摸和音量键。
- 存储：8 MB QIO Flash profile、Quad PSRAM、双 OTA 分区。
- 背光：GPIO14 LEDC，默认 60%。

ES8374、ST7789 偏移/方向、背光、音频和电源仍需真机验证；RGB LED 未实现。

引脚见 [board_config.h](board_config.h)；公共构建见[板卡索引](../README.md)，
验证状态见[验证矩阵](../../../../docs/VALIDATION.md)。
