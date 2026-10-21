# ReSpeaker Flex XVF3800 Circular-4 与 XIAO ESP32S3

本工程的板型 ID 为 `respeaker-flex-xvf3800-circular4-xiao`，面向搭配 XIAO ESP32S3 的
Circular-4 音频前端。[官方硬件文档](https://wiki.seeedstudio.com/cn/respeaker_flex_introduction/)。

## 当前配置

| 项目 | 本工程实现 |
| --- | --- |
| SoC | XIAO ESP32S3 上的 ESP32-S3 |
| Flash | 8 MB，QIO |
| PSRAM | 8 MB，Octal，80 MHz |
| 分区 | [partitions/v2/8m.csv](../../../../partitions/v2/8m.csv)，双 OTA 分区 |
| 控制台 | USB Serial/JTAG；禁用 UART0，GPIO43/GPIO44 已用于 I2S |
| 音频 | XVF3800 前端与 AIC3104 输出；SDK 使用 16 kHz 单声道 signed-16 PCM |
| 输入 | XIAO Boot GPIO0 与 XVF3800 板载按键 GPI0/X1D09 |
| 显示 | 无屏幕，不编译 LVGL |
| 摄像头 | 当前 profile 未提供视频接口，不支持开启视频 |

共享 I2C0 为 SDA GPIO5、SCL GPIO6，400 kHz；XVF3800 与 AIC3104 的 7-bit 地址分别为
`0x2c`、`0x18`。I2S0 为 BCLK GPIO8、WS GPIO7、DOUT GPIO44、DIN GPIO43。

## 音频与输入前提

使用前必须单独为 XVF3800 烧录 Circular-4 **16 kHz 双通道 I2S 固件**；本工程不会代为烧录
该前端固件。ESP32-S3 工作在 I2S 从机模式，从选定的 32-bit 采集 slot 生成 SDK 所需的
单声道 signed-16 PCM。XVF3800 提供 AEC、波束成形、AGC 与降噪，因此板级配置禁止开启
Cloud AEC，避免重复处理。音量由平台软件增益控制并保存在 NVS，初始默认值为 40。

短按任一已接入按键开始/结束对话，长按 3 秒进入配网；XVF3800 板载按键通过 I2C resource
36 轮询读取。无 Wi-Fi 信息时自动创建配置热点并播放提示音；手动配网前先停止 SDK。
设备没有屏幕，配网与配对状态通过语音和控制台日志查看。

## 构建与烧录

在仓库根目录使用 ESP-IDF v5.5.2：

```sh
source /path/to/esp-idf/export.sh
python3 scripts/build_all.py --board respeaker-flex-xvf3800-circular4-xiao --language both
```

输出 `releases/respeaker-flex-xvf3800-circular4-xiao-zh-CN.bin` 和对应英文固件。
构建脚本的清理行为见[板卡索引](../README.md)。

直接构建中文固件并烧录时，使用独立目录；将串口替换为本机 USB Serial/JTAG 设备：

```sh
idf.py -B build/respeaker-flex-xvf3800-circular4-xiao \
  -DMYBOT_BOARD=respeaker-flex-xvf3800-circular4-xiao \
  -DSDKCONFIG=build/respeaker-flex-xvf3800-circular4-xiao/sdkconfig \
  -DSDKCONFIG_DEFAULTS="sdkconfig.defaults;ci/ptime60.defaults" build
idf.py -B build/respeaker-flex-xvf3800-circular4-xiao -p /dev/ttyACM0 flash monitor
```

## 验证范围

当前 profile 尚未完成真机验收。需验证 XVF 固件版本、时钟方向、通道路由、双向音频、
声学处理、按键与重复配网。构建成功不能证明这些硬件行为正确。
详见[平台移植文档](../../../../docs/BOARD_PORTING.zh-CN.md)及
[项目说明](../../../../README.zh-CN.md)。
