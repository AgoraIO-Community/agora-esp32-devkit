# 行者 Cube 0.85 TFT Wi-Fi

板型 ID 为 `xingzhi-cube-0.85tft-wifi`。硬件参数定义在本目录的
[board_config.h](board_config.h)、[board.cmake](board.cmake) 和
[sdkconfig.defaults](sdkconfig.defaults)。

- 显示：128×128 NV3023，SPI3，共享 LVGL 紧凑布局。
- 音频：独立麦克风/扬声器 raw I2S，平台转换为 16 kHz 单声道 signed-16 PCM。
- 输入：GPIO0 Boot；无触摸和音量键。
- 存储：16 MB QIO Flash profile、80 MHz Octal PSRAM、双 OTA 分区。
- 背光：GPIO13 LEDC，默认 60%。

当前没有 codec 控制、触摸、电池、休眠或关机实现。面板初始化、方向、颜色、背光、音频和
电源仍需真机验证。

公共构建和烧录步骤见[板卡索引](../README.md)，验证状态见
[验证矩阵](../../../../docs/VALIDATION.md)。
