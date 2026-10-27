# DuChat X

板型 ID 为 `du-chatx`。

- 显示：128×160 ST7789，SPI3 mode 0，40 MHz，X/Y 偏移 2/1，共享紧凑 LVGL UI。
- 音频：独立麦克风与扬声器 raw I2S，16 kHz 单声道 signed-16 PCM。
- 输入：GPIO0 Boot；无触摸和音量键。
- 电源：GPIO1 高电平锁存，初始化时保持供电。
- 存储：16 MB QIO Flash、80 MHz Octal PSRAM、双 OTA。
- 背光：GPIO9 LEDC，默认 60%。

电池、充电状态、LED、休眠和关机未实现。物理 Flash/PSRAM 容量、GPIO1 上电时序、面板方向、
电源状态读取和音频仍需真机验证。

公共构建见[板卡索引](../README.md)，验证状态见[验证矩阵](../../../../docs/VALIDATION.md)。
