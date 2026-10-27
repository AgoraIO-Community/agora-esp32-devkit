# AIPI Lite

板型 ID 为 `aipi-lite`。

- 显示：128×128 ST7789，SPI3 mode 0，40 MHz，共享 LVGL 紧凑布局。
- 音频：ES8311，I2C1 GPIO5/4，GPIO9 speaker PA。
- 输入：GPIO42 Boot；GPIO1 电源键未接入。
- 电源：GPIO10 电源保持。
- 存储：16 MB QIO Flash、80 MHz Octal PSRAM、双 OTA。
- 背光：GPIO3 LEDC，默认 60%。

电源键动作、电池检测、休眠、关机、面板和音频仍需真机验证。

公共构建见[板卡索引](../README.md)，验证状态见[验证矩阵](../../../../docs/VALIDATION.md)。
