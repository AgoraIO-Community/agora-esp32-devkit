# ATK DNESP32S3 Box 3

板型 ID 为 `atk-dnesp32s3-box3`。

- 显示：320×240 ST7789，SPI2 mode 0，60 MHz，共享 LVGL UI。
- 音频：ES8311 输出与 ES7210 采集，16 kHz 单声道 signed-16 PCM。
- 扩展器：AW9523B `0x59` 负责音频/屏幕电源、PA 和背光。
- 输入：GPIO0 Boot；AW9523B K1/K2 未实现。
- 存储：16 MB QIO Flash、80 MHz Octal PSRAM、双 OTA。
- 背光：AW9523B bit 8，固定开启；PA 为 bit 5。

摄像头、K1/K2、电池、充电、LED、休眠和关机未实现。AW9523B 电源时序、显示数据端序、
双 codec 路由、PA 和背光仍需真机验证。

公共构建见[板卡索引](../README.md)，验证状态见[验证矩阵](../../../../docs/VALIDATION.md)。
