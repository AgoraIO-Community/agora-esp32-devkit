# M5Stack AtomEchoS3R

本工程的板型 ID 为 `atom-echos3r`，使用无屏音频 profile。

## 当前配置

| 项目 | 本工程实现 |
| --- | --- |
| SoC | ESP32-S3 |
| Flash | 8 MB，QIO |
| PSRAM | 8 MB，Octal，80 MHz |
| 分区 | [partitions/v2/8m.csv](../../../../partitions/v2/8m.csv)，双 OTA 分区 |
| 控制台 | ESP-IDF 默认 UART0，板级配置未覆盖控制台类型 |
| 音频 | ES8311 采集/播放，GPIO18 控制功放；SDK 使用 16 kHz 单声道 signed-16 PCM |
| 输入 | 当前代码映射 GPIO41 为低电平有效用户按键，映射与极性仍需真机确认 |
| 显示 | 无屏幕，不编译 LVGL |
| 摄像头 | 当前 profile 未提供视频接口，不支持开启视频 |

I2C0 使用 SDA GPIO45、SCL GPIO0，ES8311 的 7-bit 地址为 `0x18`。
I2S0 为 MCLK GPIO11、BCLK GPIO17、WS GPIO3、DIN GPIO4、DOUT GPIO48。
GPIO0 已用于 I2C 时钟，不作为配网按键。

当前代码以 GPIO41 短按开始/结束对话、长按 3 秒进入配网；该板不依赖语音唤醒启动对话。
无已保存的 Wi-Fi 信息时自动创建配置热点并播放对应语言的提示音，运行状态和配对码可通过
串口日志查看。音量逻辑值保存到 NVS，初始默认值为 70；ES8311 硬件音量按逻辑值的 60% 映射。

## 构建与烧录

在仓库根目录使用 ESP-IDF v5.5.2：

```sh
source /path/to/esp-idf/export.sh
python3 scripts/build_all.py --board atom-echos3r --language both
```

输出 `releases/atom-echos3r-zh-CN.bin` 和对应英文固件。
构建脚本的清理行为见[板卡索引](../README.md)。

直接构建中文固件并烧录时，使用独立目录；将串口替换为本机设备：

```sh
idf.py -B build/atom-echos3r \
  -DMYBOT_BOARD=atom-echos3r \
  -DSDKCONFIG=build/atom-echos3r/sdkconfig \
  -DSDKCONFIG_DEFAULTS="sdkconfig.defaults;ci/ptime60.defaults" build
idf.py -B build/atom-echos3r -p /dev/ttyUSB0 flash monitor
```

## 验证范围

GPIO41 按键映射、极性、16 kHz 采集及播放质量尚未通过本工程真机验收。
构建成功不能代替这些测试。详见[平台移植文档](../../../../docs/BOARD_PORTING.zh-CN.md)
及[项目说明](../../../../README.zh-CN.md)。
