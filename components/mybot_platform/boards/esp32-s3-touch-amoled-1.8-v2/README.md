# ESP32-S3 Touch AMOLED 1.8 V2

板型 ID 为 `esp32-s3-touch-amoled-1.8-v2`。该 profile 使用独立的 368×448 CO5300 面板配置，
不能与 1.75、1.75C 或 2.16 AMOLED profile 交叉烧录。

| 项目 | 当前实现 |
| --- | --- |
| 存储 | 16 MB QIO Flash profile；80 MHz Octal PSRAM；实际容量待真机确认 |
| 分区 | [`partitions/v2/16m.csv`](../../../../partitions/v2/16m.csv)，双 OTA |
| 电源 | TCA9554 执行面板复位；AXP2101 配置电源轨和充电参数 |
| 音频 | ES8311，16 kHz 单声道 PCM；GPIO46 扬声器功放 |
| 显示 | 368×448 CO5300 AMOLED，40 MHz QSPI，X 偏移 16，共享 LVGL |
| 输入 | CST816S 触摸与 GPIO0 Boot；短按切换对话，长按 3 秒配网 |
| 网络 / 视频 | Wi-Fi；未接入摄像头，不支持视频 |

| 信号 | GPIO |
| --- | --- |
| I2S MCLK / BCLK / WS / DIN / DOUT | 16 / 9 / 45 / 10 / 8 |
| 公共 I2C SDA / SCL | 15 / 14 |
| QSPI CLK / CS / D0-D3 | 11 / 12 / 4、5、6、7 |
| CST816S RESET / INT | 39 / 13 |
| TCA9554 / AXP2101 地址 | `0x20` / `0x34` |

逻辑界面保持 SDK 的 16 kHz 单声道 PCM 契约，I2S 与 ES8311 均运行在 16 kHz。
CO5300 使用 RGB565 局部 DMA 刷新，传输区域保持偶数像素边界。

当前未实现 AXP2101 电源键快捷行为、电量/充电上报、休眠、关机、摄像头或低功耗行为。
构建与真机状态见[验证矩阵](../../../../docs/VALIDATION.md)。
