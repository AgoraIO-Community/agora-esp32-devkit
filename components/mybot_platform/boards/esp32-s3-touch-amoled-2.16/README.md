# ESP32-S3-Touch-AMOLED-2.16

本目录提供 `agora-esp32-devkit` 的 `esp32-s3-touch-amoled-2.16` 板型配置。
硬件资料见 [Waveshare 产品页](https://www.waveshare.com/esp32-s3-touch-amoled-2.16.htm)。
该板具有独立的板级生命周期与 480 像素面板实现，不能使用 1.75 或 1.75C 的固件。

## 当前配置

| 项目 | 本工程实现 |
| --- | --- |
| 芯片 | ESP32-S3 |
| 存储 | 保守的 16 MB QIO Flash profile；80 MHz Octal PSRAM，实际容量待实物核对 |
| 分区 | `partitions/v2/16m.csv`，双 OTA 应用分区与资源分区 |
| 控制台 | 未覆盖 ESP-IDF 默认控制台设置，默认 UART0 |
| 电源 | AXP2101 初始化；GPIO46 控制扬声器功放 |
| 音频 | ES7210 采集、ES8311 播放；SDK 接口为 16 kHz、单声道、16 位 PCM |
| 显示 | 480 × 480 CO5300 AMOLED，40 MHz QSPI，无显示偏移，共享 LVGL 界面 |
| 输入 | CST9217 触摸、GPIO0 Boot 按键 |
| 网络 | Wi-Fi，支持首次自动配网和长按重新配网 |
| 视频 | 未集成摄像头采集，不支持 `CONFIG_MYBOT_ENABLE_VIDEO` |

公共 I2C0 使用 SDA GPIO15、SCL GPIO14，先完成 AXP2101 上电，再挂接屏幕、触摸和音频设备。
本 profile 可选探测地址 `0x20` 的 TCA9554，未发现时可以继续初始化；实际板上是否搭载
该器件仍需确认。

| 信号 | GPIO |
| --- | --- |
| I2S MCLK / BCLK / WS / DOUT / DIN | 42 / 9 / 45 / 8 / 10 |
| QSPI CLK / CS / D0-D3 | 38 / 12 / 4、5、6、7 |
| CO5300 reset | 39 |
| CST9217 reset / INT | 40 / 11 |

采集和播放复用 AMOLED 1.75 系列音频驱动，共享两槽 16 kHz 标准 I2S；采集提取主麦 slot，
播放将 SDK 单声道复制到两个物理输出 slot。音量可持久化，未保存过时默认值为 70。
屏幕使用独立的 `co5300_480_panel.c`，采用 RGB565 局部 DMA 刷新，传输起止边界保持
偶数像素，不分配完整 framebuffer。

## 操作与配网

就绪时轻触屏幕或短按 Boot 开始对话，对话中再次操作结束对话。长按屏幕或 Boot 约 3 秒
请求重新配网；应用先停止 SDK，再进入配网流程，联网成功后重新启动。
未保存 Wi-Fi 信息时自动进入配网，屏幕显示热点名称并播放提示音；按屏幕提示连接热点
完成配置。触摸与 Boot 由板级常驻模块管理，SDK 停止期间仍可请求配网。

## 编译与烧录

在工程根目录使用 ESP-IDF 5.5.2：

```sh
source /path/to/esp-idf/export.sh
python3 scripts/build_all.py --board esp32-s3-touch-amoled-2.16 --language both
```

脚本自动启用 LVGL，生成 `releases/esp32-s3-touch-amoled-2.16-zh-CN.bin` 和
`releases/esp32-s3-touch-amoled-2.16-en-US.bin`，默认清理脚本管理的 `build/` 和 `releases/`。

需要独立构建、烧录并查看控制台时，可使用：

```sh
idf.py -B build/esp32-s3-touch-amoled-2.16 \
  -DMYBOT_BOARD=esp32-s3-touch-amoled-2.16 \
  -DSDKCONFIG=build/esp32-s3-touch-amoled-2.16/sdkconfig build flash monitor
```

如需指定端口，在命令中增加 `-p` 参数。其他板型必须使用不同的构建目录和 sdkconfig。

## 支持边界

中英文固件构建已在 ESP-IDF 5.5.2 验证，真机验证尚未完成。发布前需核对实际 Flash/PSRAM
容量，验证电源时序、480 × 480 显示颜色/方向、触摸和 16 kHz 双向音频。若硬件需要其他
音频采样率，转换应留在平台驱动内，SDK 的 16 kHz 单声道 PCM 契约保持不变。
RTC、IMU、TF 卡、电池状态、自动休眠、关机、播放参考通道和本地 AEC 尚未集成。
完整说明见[工程文档](../../../../README.zh-CN.md)和[平台移植说明](../../../../docs/BOARD_PORTING.zh-CN.md)。
