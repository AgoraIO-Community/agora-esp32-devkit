# AtomS3R Echo Base

板型 ID 为 `atoms3r-echo-base`。

- 显示：128×128 GC9A01 类 SPI 面板，Y 偏移 32，共享 LVGL 紧凑布局。
- 音频：ES8311，16 kHz 单声道 signed-16 PCM。
- 输入：GPIO41 Boot；无触摸和音量键。
- 存储：8 MB QIO Flash profile、80 MHz Octal PSRAM、双 OTA 分区。

当前只接入显示和 ES8311 主路径。Echo Base 的外围 IO/电源管理、LP5562 背光和底座连接检测
未实现；显示、音频、按键极性和电源仍需真机验证。

引脚见 [board_config.h](board_config.h)；公共构建见[板卡索引](../README.md)，
验证状态见[验证矩阵](../../../../docs/VALIDATION.md)。
