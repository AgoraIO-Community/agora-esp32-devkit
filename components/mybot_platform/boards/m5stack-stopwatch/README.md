# M5Stack StopWatch

板型 ID 为 `m5stack-stopwatch`。这是 466×466 圆屏 profile，使用共享 LVGL 中央安全区布局。

- 显示：CO5300 QSPI，466×466，内部亮度控制。
- 音频：ES8311 `0x18`，16 kHz 单声道 signed-16 PCM。
- 输入：CST820B 兼容触摸路径、GPIO1 主按键、GPIO2 音量按键。
- 电源/扩展：M5PM1 `0x6E`、M5IOE1 `0x4F`，负责充电、LCD/codec 电源、复位、
  speaker PA 和电机默认关闭。
- 存储：16 MB QIO Flash profile、80 MHz Octal PSRAM、双 OTA 分区。

M5PM1/M5IOE1 时序、触摸协议、圆屏裁切、音频和按键行为尚未真机验证。电池上报、振动输出、
休眠和关机尚未实现。

详细引脚见 [board_config.h](board_config.h)；公共构建见[板卡索引](../README.md)，
验证状态见[验证矩阵](../../../../docs/VALIDATION.md)。
