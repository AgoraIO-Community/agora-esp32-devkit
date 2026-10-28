# KEVIN Box 2

板型 ID 为 `kevin-box-2`。当前实现为 Wi-Fi headless 子集。

- 显示：未启用；128×64 SSD1306 单色 UI 尚未实现。
- 音频：ES8311 输出与 ES7210 采集，16 kHz 单声道 signed-16 PCM。
- 电源：AXP2101 `0x34`，GPIO9 speaker PA。
- 输入：GPIO0 Boot、GPIO1 音量加、GPIO2 音量减。
- 存储：16 MB QIO Flash、80 MHz Octal PSRAM、双 OTA。

ML307/4G、OLED、LED、电池状态、休眠和关机未实现。AXP2101 电源时序、双 codec 路由和按键
仍需真机验证。

公共构建见[板卡索引](../README.md)，验证状态见[验证矩阵](../../../../docs/VALIDATION.md)。
