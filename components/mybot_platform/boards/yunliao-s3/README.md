# Yunliao S3

板型 ID 为 `yunliao-s3`。

- 显示：320×240 ST7789，SPI2 mode 0，40 MHz，共享 LVGL UI。
- 音频：ES8388，16 kHz 单声道 signed-16 PCM，GPIO17 控制 PA。
- 输入：GPIO2 Boot。
- 存储：16 MB QIO Flash、80 MHz Octal PSRAM、双 OTA。
- 背光：GPIO46 LEDC，默认 60%。

本 profile 仅启用 Wi-Fi；ML307/4G、电池、USB 和蓝牙状态检测未实现。ES8388、PA、面板方向和
Wi-Fi 恢复仍需真机验证。

公共构建见[板卡索引](../README.md)，验证状态见[验证矩阵](../../../../docs/VALIDATION.md)。
