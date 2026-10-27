# DOIT S3 AI Box

板型 ID 为 `doit-s3-aibox`。当前为 headless profile，不编入 LVGL。

- 音频：PDM 采集 GPIO40/42，标准 I2S 播放 GPIO17/16/18。
- 输入：GPIO10 主按键、GPIO15 音量加、GPIO9 音量减。
- 电源：GPIO48 进程级保持。
- 存储：16 MB QIO Flash、80 MHz Octal PSRAM、双 OTA。

PDM 时钟、录音增益、播放、按键极性和电源仍需真机验证；LED、休眠和关机未实现。

公共构建见[板卡索引](../README.md)，验证状态见[验证矩阵](../../../../docs/VALIDATION.md)。
