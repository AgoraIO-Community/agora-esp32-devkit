# SP ESP32-S3 1.28 Box

板型 ID 为 `sp-esp32-s3-1.28-box`。

- 显示：240×240 圆屏 GC9A01，SPI3，共享 LVGL 圆屏安全区布局。
- 音频：ES8311，16 kHz 单声道 signed-16 PCM。
- 输入：CST816S 兼容触摸、GPIO0 Boot；触摸使用独立 I2C1 GPIO11/7。
- 存储：16 MB QIO Flash profile、80 MHz Octal PSRAM、双 OTA 分区。
- 背光：GPIO42 LEDC，默认 60%。

触摸复位/中断、面板厂商初始化、颜色、方向、背光、音频和电源仍需真机验证。电池/充电上报
未实现。

引脚见 [board_config.h](board_config.h)；公共构建见[板卡索引](../README.md)，
验证状态见[验证矩阵](../../../../docs/VALIDATION.md)。
