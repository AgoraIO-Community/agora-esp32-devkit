# ESP32-S3-Touch-AMOLED-1.75

本目录提供 `mybot-esp32` 的 `esp32-s3-touch-amoled-1.75` 板型配置。
硬件资料见 [Waveshare 产品页](https://www.waveshare.com/esp32-s3-touch-amoled-1.75.htm)。
本 profile 用于原始 1.75 版，不能与 1.75C 或 2.16 的固件交叉烧录。

## 当前配置

| 项目 | 本工程实现 |
| --- | --- |
| 芯片 | ESP32-S3 |
| 存储 | 16 MB QIO Flash；8 MB Octal PSRAM，80 MHz |
| 分区 | `partitions/v2/16m.csv`，双 OTA 应用分区与资源分区 |
| 控制台 | 未覆盖 ESP-IDF 默认控制台设置，默认 UART0 |
| 电源 | AXP2101 初始化；GPIO46 控制扬声器功放 |
| 音频 | ES7210 采集、ES8311 播放；SDK 接口为 16 kHz、单声道、16 位 PCM |
| 显示 | 466 × 466 CO5300 AMOLED，40 MHz QSPI，显示偏移 `(6, 0)`，共享 LVGL 界面 |
| 输入 | CST9217 触摸、GPIO0 Boot 按键 |
| 网络 | Wi-Fi，支持首次自动配网和长按重新配网 |
| 视频 | 未集成摄像头采集，不支持 `CONFIG_MYBOT_ENABLE_VIDEO` |

公共 I2C0 使用 SDA GPIO15、SCL GPIO14，先完成 AXP2101 上电，再挂接屏幕、触摸和音频设备。
本板可选探测地址 `0x20` 的 TCA9554，未发现该器件时可以继续初始化。

| 信号 | GPIO |
| --- | --- |
| I2S MCLK / BCLK / WS / DOUT / DIN | 42 / 9 / 45 / 8 / 10 |
| QSPI CLK / CS / D0-D3 | 38 / 12 / 4、5、6、7 |
| CO5300 reset | 39 |
| CST9217 reset / INT | 40 / 11 |

采集和播放共享两槽 16 kHz 标准 I2S；采集提取主麦 slot，播放将 SDK 单声道复制到两个物理
输出 slot。音量可持久化，未保存过时默认值为 70。屏幕使用 RGB565 局部 DMA 刷新，传输
起止边界保持偶数像素，不分配完整 framebuffer。

## 操作与配网

就绪时轻触屏幕或短按 Boot 开始对话，对话中再次操作结束对话。长按屏幕或 Boot 约 3 秒
请求重新配网；应用先停止 MyBot，再进入配网流程，联网成功后重新启动。
未保存 Wi-Fi 信息时自动进入配网，屏幕显示热点名称并播放提示音；按屏幕提示连接热点
完成配置。触摸与 Boot 由板级常驻模块管理，MyBot 停止期间仍可请求配网。

本板与 1.75C 共用生命周期、音频和输入实现，但 MCLK、屏幕复位、触摸复位和 TCA9554
配置不同。构建、烧录与验证状态见[板卡索引](../README.md)和
[验证矩阵](../../../../docs/VALIDATION.md)。
