# SenseCAP Watcher

本工程的板型 ID 为 `sensecap-watcher`。[English](README_en.md)。

## 当前配置

| 项目 | 本工程实现 |
| --- | --- |
| SoC | ESP32-S3 |
| Flash | 32 MB，QIO，开启 Quad Flash 32-bit 地址支持 |
| PSRAM | Octal，80 MHz；容量由实际设备检测 |
| 分区 | [partitions/v2/32m-sensecap.csv](../../../../partitions/v2/32m-sensecap.csv)，保留出厂数据并提供双 4 MB OTA 分区 |
| 控制台 | ESP-IDF 默认 UART0，板级配置未覆盖控制台类型 |
| 音频 | ES7243E 采集、ES8311 播放；SDK 边界为 16 kHz 单声道 signed-16 PCM |
| 显示 | 412 × 412 SPD2010，QSPI，自动使用共享 LVGL 界面 |
| 输入 | 编码器 GPIO41/GPIO42，按键由 TCA9555 P0.3 读取 |
| 电源 | TCA9555 管理系统、LCD 与 codec/功放电源 |
| 摄像头 | 当前 profile 未提供视频接口，不支持开启视频 |

共享 I2C0 为 SDA GPIO47、SCL GPIO48，TCA9555 的 7-bit 地址为 `0x21`。
I2S0 为 MCLK GPIO10、BCLK GPIO11、WS GPIO12、DIN GPIO15、DOUT GPIO16。
LCD QSPI 为 CLK GPIO7、D0 GPIO9、D1 GPIO1、D2 GPIO14、D3 GPIO13、CS GPIO45，
背光为 GPIO8。局部刷新按 4 像素对齐。

短按编码器开始/结束对话，长按 3 秒进入配网；代码将左旋映射为音量增加、右旋映射为音量
降低，旋转方向需实机确认。音量保存到 NVS。无 Wi-Fi 信息时自动进入配网并显示热点名称、
播放提示音；手动配网前先停止 SDK。

## 出厂数据保护

为保留出厂分区，设备必须使用[板卡索引](../README.md)中的独立构建与 `idf.py flash`，
不要直接向整片 Flash 写入合并镜像。

首次烧录前备份出厂分区，将串口替换为本机设备，并妥善保存备份：

```sh
python -m esptool --chip esp32s3 --port /dev/ttyUSB0 --baud 2000000 \
  --before default_reset --after hard_reset --no-stub \
  read_flash 0x9000 204800 nvsfactory.bin
```

出厂分区 `nvsfactory` 从 `0x9000` 开始，长度为 200 KiB。禁止执行 `erase-flash` 或使用
其他板型的分区表。标准 `idf.py flash` 不会写入 `nvsfactory`。构建与验证状态见
[板卡索引](../README.md)和[验证矩阵](../../../../docs/VALIDATION.md)。
