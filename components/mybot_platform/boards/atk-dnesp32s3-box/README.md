# ATK DNESP32S3 Box

板型 ID 为 `atk-dnesp32s3-box`。

- 显示：320×240 ST7789，i80 8-bit 总线，10 MHz，共享 LVGL UI。
- 音频：ES8311，16 kHz 单声道 signed-16 PCM。
- 扩展器：TCA9555/XL9555 `0x20` 控制系统和功放供电。
- 输入：GPIO0 Boot。
- 存储：16 MB QIO Flash、80 MHz Octal PSRAM、双 OTA。

仅支持检测到 ES8311 的硬件版本；NS4168 无 codec 音频变体未实现。扩展器按键、摄像头、电池、
调光、休眠和关机未实现。i80 时序、ES8311、扩展器供电和面板方向仍需真机验证。

公共构建见[板卡索引](../README.md)，验证状态见[验证矩阵](../../../../docs/VALIDATION.md)。
