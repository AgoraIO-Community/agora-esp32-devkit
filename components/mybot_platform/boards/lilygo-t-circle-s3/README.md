# LilyGO T-Circle-S3

板型 ID 为 `lilygo-t-circle-s3`。

- 显示：160×160 GC9D01N 圆屏，SPI3 mode 0，40 MHz，共享 LVGL 圆屏布局。
- 音频：MSM261 标准 I2S 采集和 MAX98357A 播放，16 kHz 单声道 signed-16 PCM。
- 触摸：CST816D，I2C GPIO11/GPIO14，中断 GPIO12。
- 输入：GPIO0 Boot；GPIO45 控制扬声器使能。
- 存储：16 MB QIO Flash、80 MHz Octal PSRAM、双 OTA。
- 背光：GPIO18，默认 60%。

旋转编码器、APA102 灯、电池、休眠和关机未实现。GC9D01N 初始化、圆屏方向、触摸和双 I2S 音频
仍需真机验证。

公共构建见[板卡索引](../README.md)，验证状态见[验证矩阵](../../../../docs/VALIDATION.md)。
