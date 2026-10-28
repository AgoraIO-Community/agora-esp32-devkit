# Minsi K08 Dual

板型 ID 为 `minsi-k08-dual`。当前实现仅使用 Wi-Fi 通路。

- 显示：240×240 ST7789，SPI3 mode 3，40 MHz，X 偏移 80，共享 LVUI。
- 音频：独立麦克风与扬声器 raw I2S，16 kHz 单声道 signed-16 PCM。
- 输入：GPIO0 Boot、GPIO47 音量加、GPIO46 音量减。
- 存储：16 MB QIO Flash、80 MHz Octal PSRAM、双 OTA。
- 背光：GPIO7 LEDC，默认 60%。

ML307/4G、电池、充电、LED、测试灯、休眠和关机未实现。GPIO18 同时被上游代码定义为
麦克风 DIN 和测试灯，本 profile 不启用测试灯。显示方向、音频和电源仍需真机验证。

公共构建见[板卡索引](../README.md)，验证状态见[验证矩阵](../../../../docs/VALIDATION.md)。
