# 征辰 Camera

板型 ID 为 `zhengchen-cam`。

- 显示：320×240 ST7789，SPI3 mode 0，80 MHz，固定横屏，共享 LVGL UI。
- 音频：ES8311 输出与 ES7210 采集，16 kHz 单声道 signed-16 PCM。
- 扩展器：PCA9557 `0x19` 控制 speaker PA。
- 输入：GPIO0 Boot、GPIO3 音量加、GPIO46 音量减。
- 存储：16 MB QIO Flash、80 MHz Octal PSRAM、双 OTA。
- 背光：GPIO42 LEDC，低电平有效，默认 60%。

摄像头、竖屏切换、电池、温度、休眠和关机未实现。PCA9557 位映射、面板方向、双 codec
路由、PA 和音频仍需真机验证。

公共构建见[板卡索引](../README.md)，验证状态见[验证矩阵](../../../../docs/VALIDATION.md)。
