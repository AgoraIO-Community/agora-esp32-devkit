# M5Stack CoreS3

本工程的板型 ID 为 `m5stack-core-s3`，面向带 GC0308 摄像头的 CoreS3。
[官方硬件文档](https://docs.m5stack.com/en/core/CoreS3)。

## 当前配置

| 项目 | 本工程实现 |
| --- | --- |
| SoC | ESP32-S3 |
| Flash | 16 MB，QIO |
| PSRAM | 8 MB，Quad，80 MHz |
| 分区 | [partitions/v2/16m.csv](../../../../partitions/v2/16m.csv)，双 OTA 分区 |
| 控制台 | ESP-IDF 默认 UART0，板级配置未覆盖控制台类型 |
| 音频 | ES7210 采集、AW88298 播放；SDK 边界为 16 kHz 单声道 signed-16 PCM |
| 显示 | 320 × 240 ILI9342，自动使用 LVGL 像素眼镜界面 |
| 输入 | FT6336 触摸：短触开始/结束对话，长按 3 秒进入配网 |
| 摄像头 | GC0308 DVP，320 × 240 YUYV，软件 JPEG，上行最多 1 fps |
| 电源 | AXP2101 与 AW9523，由板级硬件层初始化并控制背光、复位 |

共享 I2C1 为 SDA GPIO12、SCL GPIO11。I2S0 为 MCLK GPIO0、BCLK GPIO34、WS GPIO33、
DIN GPIO14、DOUT GPIO13；GPIO0 用于音频时钟，不作为配网按键。LCD SPI3 为 MOSI GPIO37、
SCLK GPIO36、CS GPIO3、DC GPIO35。

无已保存的 Wi-Fi 信息时自动进入配网并显示热点名称、播放提示音。已有网络时联网后启动
MyBot；长按触摸会先停止 MyBot 再配网。设备音量保存在 NVS，重启后恢复。

## 构建与烧录

在仓库根目录使用 ESP-IDF v5.5.2：

```sh
source /path/to/esp-idf/export.sh
python3 scripts/build_all.py --board m5stack-core-s3 --language both
```

脚本默认开启视频，输出 `releases/m5stack-core-s3-zh-CN-video.bin` 和对应英文固件。
需要关闭视频时追加 `--video off`。CoreS3-SE 不带 GC0308，不能使用视频固件。
构建脚本的清理行为见[板卡索引](../README.md)。

直接构建中文视频固件并烧录时，使用独立目录；将串口替换为本机设备：

```sh
idf.py -B build/m5stack-core-s3 \
  -DMYBOT_BOARD=m5stack-core-s3 \
  -DSDKCONFIG=build/m5stack-core-s3/sdkconfig \
  -DSDKCONFIG_DEFAULTS="sdkconfig.defaults;ci/ptime60.defaults;ci/video.defaults" build
idf.py -B build/m5stack-core-s3 -p /dev/ttyUSB0 flash monitor
```

英文直接构建需在新目录的 defaults 列表追加 `;ci/en-us.defaults`。

## 验证范围

已有音频和摄像头通路完成过真机验证；当前像素眼镜 UI、采集调度和 DMA 配置仍需按对应
文档回归。构建成功不等于硬件验收。详见[像素眼镜 UI](../../../../docs/CORES3_UI.zh-CN.md)、
[摄像头上行](../../../../docs/CORES3_VIDEO.zh-CN.md)及
[音频对照测试](../../../../docs/CORES3_AUDIO_TEST.zh-CN.md)。
