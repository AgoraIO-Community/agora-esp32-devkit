# 行者 Metal 1.54 Wi-Fi

板型 ID 为 `xingzhi-metal-1.54-wifi`。引脚和能力以 [board_config.h](board_config.h)、
[board.cmake](board.cmake) 与 [sdkconfig.defaults](sdkconfig.defaults) 为准。

- 显示：240×240 ST7789，SPI3 mode 3，共享 LVGL UI。
- 音频：ES8311，16 kHz 单声道 signed-16 PCM；I2C0 GPIO41/42。
- 输入：PCA9557 `0x19`、CST816S 触摸和 GPIO0 Boot。
- 存储：16 MB QIO Flash profile、80 MHz Octal PSRAM、双 OTA 分区。
- 电源：GPIO48 进程级电源保持；实际极性和上电时序待真机确认。

当前未实现电池/充电上报、休眠和关机；PCA9557 电源/复位、触摸、PA 和面板仍需真机验证。

公共构建、烧录和验证入口见[板卡索引](../README.md)与
[验证矩阵](../../../../docs/VALIDATION.md)。
