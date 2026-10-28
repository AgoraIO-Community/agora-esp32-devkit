# LabPlus 掌控板 V3

板型 ID 为 `labplus-mpython-v3`。

- 显示：320×172 ST7789 兼容 SPI 面板，SPI2 mode 0，20 MHz，大端像素数据。
- 音频：ES8388，I2C0 GPIO44/43，16 kHz 单声道 signed-16 PCM。
- 输入：GPIO0 Boot；第二个按键 GPIO46 未接入。
- 存储：16 MB QIO Flash、80 MHz Quad PSRAM、双 OTA。
- 背光：GPIO33 LEDC，默认 60%。

触摸、传感器、蜂鸣器、RGB 灯、电机、摄像头、休眠和关机未实现。ES8388、面板方向和音频
仍需真机验证。

公共构建见[板卡索引](../README.md)，验证状态见[验证矩阵](../../../../docs/VALIDATION.md)。
