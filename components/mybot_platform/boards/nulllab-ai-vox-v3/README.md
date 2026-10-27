# NullLab AI Vox V3

板型 ID 为 `nulllab-ai-vox-v3`。

- 显示：240×240 ST7789，SPI3 mode 0，40 MHz，共享 LVGL UI。
- 音频：ES8311，MCLK GPIO11、BCLK GPIO10、WS GPIO8、DIN GPIO9、DOUT GPIO7。
- 输入：GPIO0 Boot、GPIO46 音量加、GPIO45 音量减。
- 存储：16 MB QIO Flash、80 MHz Octal PSRAM、双 OTA。
- 背光：GPIO16 LEDC，默认 60%。

当前只实现 Wi-Fi 子集；ML307 蜂窝网络未接入。面板、音频、PA、电池和按键仍需真机验证。

公共构建见[板卡索引](../README.md)，验证状态见[验证矩阵](../../../../docs/VALIDATION.md)。
