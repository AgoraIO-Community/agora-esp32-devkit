# ESP-Box

板型 ID 为 `esp-box`。

- 显示：320×240 ILI9341，SPI3，共享 LVGL UI。
- 音频：ES8311 输出与 ES7210 采集，16 kHz 单声道 signed-16 PCM。
- 输入：GPIO0 Boot；当前未实现触摸。
- 存储：16 MB QIO Flash profile、80 MHz Octal PSRAM、双 OTA 分区。
- 背光：GPIO45 LEDC，默认 60%。

ILI9341 厂商初始化、双 codec 路由、触摸、板级 PMIC/电源时序和音频仍需真机验证。

引脚见 [board_config.h](board_config.h)；公共构建见[板卡索引](../README.md)，
验证状态见[验证矩阵](../../../../docs/VALIDATION.md)。
