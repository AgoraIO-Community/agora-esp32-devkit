# KEVIN SP V4 Dev

板型 ID 为 `kevin-sp-v4-dev`。

- 显示：240×280 ST7789，SPI3 mode 3，80 MHz，Y 偏移 20，颜色反转，共享 LVGL UI。
- 音频：ES8311，I2C1 GPIO4/5，GPIO41 speaker PA，16 kHz 单声道 signed-16 PCM。
- 输入：GPIO0 Boot；无触摸和音量键。
- 存储：16 MB QIO Flash、80 MHz Octal PSRAM、双 OTA。
- 背光：GPIO48 LEDC，默认 60%。

摄像头、LED、电池、休眠和关机未实现。物理 Flash/PSRAM 容量、I2C1 上的 ES8311、面板方向、
音频和按键仍需真机验证。

公共构建见[板卡索引](../README.md)，验证状态见[验证矩阵](../../../../docs/VALIDATION.md)。
