# Tudouzi

板型 ID 为 `tudouzi`。

- 显示：当前为无屏 profile；128×64 SSD1306 单色 UI 未实现。
- 音频：ES8311 输出与 ES7210 采集，16 kHz 单声道 signed-16 PCM，GPIO9 控制 PA。
- 电源：AXP2101 `0x34` 配置 ALDO2、充电参数和系统欠压阈值。
- 输入：GPIO0 主按键，GPIO1/GPIO2 音量加减。
- 存储：16 MB QIO Flash、80 MHz Octal PSRAM、双 OTA。

仅启用 Wi-Fi；ML307/4G、SSD1306、LED、电池状态、休眠和关机未实现。AXP2101、双 codec、
PA 和按键仍需真机验证。

公共构建见[板卡索引](../README.md)，验证状态见[验证矩阵](../../../../docs/VALIDATION.md)。
