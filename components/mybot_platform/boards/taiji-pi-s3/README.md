# Taiji Pi S3

板型 ID 为 `taiji-pi-s3`。

- 显示：360×360 圆屏 ST77916 QSPI，共享 LVGL 圆屏安全区布局。
- 音频：I2S 播放和 PDM 采集；PDM 解调后直接提供 16 kHz 单声道 signed-16 PCM。
- 输入：CST816S 触摸和 GPIO0 Boot；触摸使用独立 I2C1 GPIO7/8。
- 静音/PA：GPIO48 低有效，随播放启停。
- 存储：16 MB QIO Flash profile、80 MHz Octal PSRAM、双 OTA 分区。

ST77916 默认初始化、PDM 时钟/增益、I2S 播放、mute/PA、触摸、背光和板级电源仍需真机验证。
完整板级电源时序尚未实现。

引脚见 [board_config.h](board_config.h)；公共构建见[板卡索引](../README.md)，
验证状态见[验证矩阵](../../../../docs/VALIDATION.md)。
