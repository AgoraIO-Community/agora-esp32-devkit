# ESP-Box Lite

板型 ID 为 `esp-box-lite`。

- 显示：320×240 ILI9341，SPI3，共享 LVGL UI。
- 音频：当前按 ES8311 输出与 ES7210 采集 profile 接入，16 kHz 单声道 signed-16 PCM。
- 输入：GPIO0 Boot；ADC 按键尚未实现。
- 存储：16 MB QIO Flash profile、80 MHz Octal PSRAM、双 OTA 分区。
- 背光：GPIO45 LEDC；LCD reset 为低有效。

实际 codec 型号、ADC 按键、ILI9341 初始化、电源时序和音频仍需真机核对。若实物 codec 与
当前 profile 不同，应新增独立驱动，不得沿用错误配置。

引脚见 [board_config.h](board_config.h)；公共构建见[板卡索引](../README.md)，
验证状态见[验证矩阵](../../../../docs/VALIDATION.md)。
