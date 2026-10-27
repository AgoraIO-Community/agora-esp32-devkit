# ESP32-S3 Touch LCD 1.85

板型 ID 为 `esp32-s3-touch-lcd-1.85`。该 profile 使用 ST77916 QSPI 面板、双 I2C 总线、
TCA9554 复位控制和独立麦克风/扬声器 I2S。

| 项目 | 当前实现 |
| --- | --- |
| 存储 | 16 MB QIO Flash profile；80 MHz Octal PSRAM；实际容量待真机确认 |
| 分区 | [`partitions/v2/16m.csv`](../../../../partitions/v2/16m.csv)，双 OTA |
| 电源保持 | GPIO7 在进程运行期间保持高电平 |
| 音频 | 原始 I2S；16 kHz 单声道 PCM；麦克风读取右 slot，播放复制到两个 slot |
| 显示 | 360×360 ST77916，80 MHz QSPI，共享 LVGL |
| 输入 | CST816S 触摸与 GPIO0 Boot；短按切换对话，长按 3 秒配网 |
| 网络 / 视频 | Wi-Fi；未接入摄像头，不支持视频 |

| 信号 | GPIO |
| --- | --- |
| 麦克风 I2S RX | WS 2、BCLK 15、DIN 39 |
| 扬声器 I2S TX | DOUT 47、BCLK 48、WS 38 |
| 控制 I2C SDA / SCL | 11 / 10，TCA9554 地址 `0x20` |
| 触摸 I2C SDA / SCL | 1 / 3，中断 GPIO4 |
| QSPI CLK / CS / D0-D3 | 40 / 21 / 46、45、42、41 |
| LCD 背光 | 5，LEDC 5 kHz、13-bit，默认 60% |

TCA9554 的 bit0/bit1 用于释放 LCD 与触摸复位。当前 profile 不映射硬件参考中的 GPIO6
电源按键，也未实现电量、充电上报、休眠、关机或低功耗行为。构建与真机状态见
[验证矩阵](../../../../docs/VALIDATION.md)。
