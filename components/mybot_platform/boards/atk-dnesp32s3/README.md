# ATK DNESP32S3

板型 ID 为 `atk-dnesp32s3`。

- 显示：320×240 ST7789，SPI2 mode 0，20 MHz，大端像素数据，共享 LVGL UI。
- 音频：ES8388，16 kHz 单声道 signed-16 PCM。
- 电源：TCA9555 `0x20` 打开 LCD 电源并配置相关输出。
- 输入：GPIO0 Boot。
- 存储：16 MB QIO Flash、80 MHz Octal PSRAM、双 OTA。

摄像头、扩展按键、背光调节、电池、休眠和关机未实现。ES8388、TCA9555 电源控制和面板方向仍需
真机验证。

公共构建见[板卡索引](../README.md)，验证状态见[验证矩阵](../../../../docs/VALIDATION.md)。
