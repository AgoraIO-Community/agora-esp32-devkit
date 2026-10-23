# ESP-VoCat

本目录提供 `mybot-esp32` 的 `esp-vocat` 板型配置，支持 PCB V1.0 和 V1.2。
两个版本使用相同的固件，启动时先探测硬件版本，再初始化音频、显示和输入。
硬件资料见 [ESP-VoCat 开源硬件项目](https://oshwhub.com/esp-college/echoear)。

## 当前配置

| 项目 | 本工程实现 |
| --- | --- |
| 芯片 | ESP32-S3 |
| 存储 | 16 MB QIO Flash；80 MHz Octal PSRAM，实际容量以启动日志为准 |
| 分区 | `partitions/v2/16m.csv`，双 OTA 应用分区与资源分区 |
| 控制台 | 原生 USB Serial/JTAG；关闭 UART0 控制台，避免与外设引脚冲突 |
| 音频 | ES7210 采集、ES8311 播放；SDK 接口为 16 kHz、单声道、16 位 PCM |
| 显示 | 360 × 360 ST77916 圆屏，40 MHz QSPI，共享 LVGL 界面 |
| 输入 | CST816S 触摸、GPIO0 Boot 按键 |
| 网络 | Wi-Fi，支持首次自动配网和长按重新配网 |
| 视频 | 未集成摄像头采集，不支持 `CONFIG_MYBOT_ENABLE_VIDEO` |

公共 I2C0 使用 SDA GPIO2、SCL GPIO1。I2S 使用 MCLK GPIO42、BCLK GPIO40、
WS GPIO39、DOUT GPIO41；DIN、功放使能和屏幕复位由 PCB 版本决定：

| PCB 版本 | I2S DIN | 功放使能 | ST77916 reset |
| --- | --- | --- | --- |
| V1.0 | GPIO15 | GPIO4 | GPIO3，低有效 |
| V1.2 | GPIO3 | GPIO15 | GPIO47，高有效 |

GPIO9 以低电平启用外设电源。初始化时先将 GPIO48 拉低并探测 ES8311，失败后拉高重试；
两次均失败则终止硬件初始化，不使用默认版本继续启动。QSPI 使用 CLK GPIO18、CS GPIO14、
D0-D3 GPIO46/GPIO13/GPIO11/GPIO12，背光为 GPIO44，触摸中断为 GPIO10。

采集和播放共享两槽 I2S 时钟，直接运行在 16 kHz：采集只提取主麦 slot，播放将 SDK
单声道复制到两个物理输出 slot。音量可持久化，未保存过时默认值为 70。

## 操作与配网

就绪时轻触屏幕或短按 Boot 开始对话，对话中再次操作结束对话。长按屏幕或 Boot 约 3 秒
请求重新配网；应用先停止 MyBot，再进入配网流程，联网成功后重新启动。
未保存 Wi-Fi 信息时自动进入配网，屏幕显示热点名称并播放提示音；按屏幕提示连接热点
完成配置。

CST816S 使用中断触发读取，仅在按住时定时检查长按，不持续轮询空闲触摸。当前配置关闭
触摸 ID 寄存器读取，以兼容不响应该寄存器的固件批次。

烧录时使用原生 USB 并打开硬件电源。构建、烧录与验证状态见[板卡索引](../README.md)和
[验证矩阵](../../../../docs/VALIDATION.md)。
