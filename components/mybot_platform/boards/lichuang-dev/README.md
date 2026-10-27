# Lichuang Dev

板型 ID 为 `lichuang-dev`。

- 显示：320×240 ST7789，SPI3 mode 2，共享 LVGL UI。
- 音频：ES8311 输出与 ES7210 采集，16 kHz 单声道 signed-16 PCM。
- 输入：FT5x06 触摸和 GPIO0 Boot。
- 扩展：PCA9557 `0x19`，控制显示复位和 speaker PA。
- 存储：16 MB QIO Flash profile、80 MHz Octal PSRAM、双 OTA 分区。

PCA9557 输出映射、FT5x06 坐标、ST7789 初始化、双 codec 路由、音频和背光仍需真机验证。
摄像头和电池上报未实现。

引脚见 [board_config.h](board_config.h)；公共构建见[板卡索引](../README.md)，
验证状态见[验证矩阵](../../../../docs/VALIDATION.md)。
