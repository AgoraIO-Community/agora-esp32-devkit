# ESP32-S3-Touch-AMOLED-1.75C

本目录提供 `agora-esp32-devkit` 的 `esp32-s3-touch-amoled-1.75c` 板型配置。
该板使用独立的引脚 profile，与原始 1.75 和 2.16 版本不能交叉烧录。

## 当前配置

| 项目 | 本工程实现 |
| --- | --- |
| 芯片 | ESP32-S3 |
| 存储 | 保守的 16 MB QIO Flash profile；8 MB Octal PSRAM，80 MHz |
| 分区 | `partitions/v2/16m.csv`，双 OTA 应用分区与资源分区 |
| 控制台 | 未覆盖 ESP-IDF 默认控制台设置，默认 UART0 |
| 电源 | AXP2101 初始化；GPIO46 控制扬声器功放；不探测或依赖 TCA9554 |
| 音频 | ES7210 采集、ES8311 播放；SDK 接口为 16 kHz、单声道、16 位 PCM |
| 显示 | 466 × 466 CO5300 AMOLED，40 MHz QSPI，显示偏移 `(6, 0)`，共享 LVGL 界面 |
| 输入 | CST9217 触摸、GPIO0 Boot 按键 |
| 网络 | Wi-Fi，支持首次自动配网和长按重新配网 |
| 视频 | 未集成摄像头采集，不支持 `CONFIG_MYBOT_ENABLE_VIDEO` |

公共 I2C0 使用 SDA GPIO15、SCL GPIO14，先完成 AXP2101 上电，再挂接屏幕、触摸和音频设备。
1.75C 的存储资料存在容量差异，因此固件只使用 16 MB 地址空间；扩展分区前必须通过启动
日志确认实际 Flash 容量并验证分区布局。

| 信号 | 1.75C GPIO | 原始 1.75 GPIO |
| --- | --- | --- |
| I2S MCLK | 16 | 42 |
| CO5300 reset | 1 | 39 |
| CST9217 reset | 2 | 40 |

其他公共信号为 I2S BCLK GPIO9、WS GPIO45、DOUT GPIO8、DIN GPIO10，QSPI CLK GPIO38、
CS GPIO12、D0-D3 GPIO4/GPIO5/GPIO6/GPIO7，以及 CST9217 INT GPIO11。

采集和播放共享两槽 16 kHz 标准 I2S；采集提取主麦 slot，播放将 SDK 单声道复制到两个物理
输出 slot。音量可持久化，未保存过时默认值为 70。屏幕使用 RGB565 局部 DMA 刷新，传输
起止边界保持偶数像素，不分配完整 framebuffer。

## 操作与配网

就绪时轻触屏幕或短按 Boot 开始对话，对话中再次操作结束对话。长按屏幕或 Boot 约 3 秒
请求重新配网；应用先停止 SDK，再进入配网流程，联网成功后重新启动。
未保存 Wi-Fi 信息时自动进入配网，屏幕显示热点名称并播放提示音；按屏幕提示连接热点
完成配置。触摸与 Boot 由板级常驻模块管理，SDK 停止期间仍可请求配网。

## 编译与烧录

在工程根目录使用 ESP-IDF 5.5.2：

```sh
source /path/to/esp-idf/export.sh
python3 scripts/build_all.py --board esp32-s3-touch-amoled-1.75c --language both
```

脚本自动启用 LVGL，生成 `releases/esp32-s3-touch-amoled-1.75c-zh-CN.bin` 和
`releases/esp32-s3-touch-amoled-1.75c-en-US.bin`，默认清理脚本管理的 `build/` 和 `releases/`。

需要独立构建、烧录并查看控制台时，可使用：

```sh
idf.py -B build/esp32-s3-touch-amoled-1.75c \
  -DMYBOT_BOARD=esp32-s3-touch-amoled-1.75c \
  -DSDKCONFIG=build/esp32-s3-touch-amoled-1.75c/sdkconfig build flash monitor
```

如需指定端口，在命令中增加 `-p` 参数。其他板型必须使用不同的构建目录和 sdkconfig。

## 支持边界

本板复用 `esp32-s3-touch-amoled-1.75-common` 的板级生命周期、音频和输入实现，保留独立
板型身份。RTC、IMU、TF 卡、电池状态、自动休眠、关机、播放参考通道和本地 AEC 尚未集成。
当前支持范围为构建集成；电源时序、音频 slot、显示颜色/方向和触摸仍需在匹配实物上验证。
完整说明见[工程文档](../../../../README.zh-CN.md)和[平台移植说明](../../../../docs/BOARD_PORTING.zh-CN.md)。
