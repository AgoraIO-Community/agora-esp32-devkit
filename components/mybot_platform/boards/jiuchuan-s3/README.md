# Jiuchuan S3

板型 ID 为 `jiuchuan-s3`。

- 显示：240×296 GC9309NA，SPI3 mode 3，80 MHz，BGR 顺序，共享 LVGL UI。
- 音频：ES8311，16 kHz 单声道 signed-16 PCM，GPIO42 控制 PA。
- 输入：GPIO0 主按键，GPIO6/GPIO7 音量加减。
- 电源：GPIO5 保持高电平；不执行关机或休眠。
- 存储：16 MB QIO Flash、80 MHz Octal PSRAM、双 OTA。
- 背光：GPIO46 LEDC，默认 60%。

电源键 GPIO3 的单击、长按关机、电池和充电检测未实现。GC9309NA 初始化、ES8311、PA、按键和
面板方向仍需真机验证。

公共构建见[板卡索引](../README.md)，验证状态见[验证矩阵](../../../../docs/VALIDATION.md)。
