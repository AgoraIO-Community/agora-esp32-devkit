# Espressif ESP-VoCat 喵伴

本目录是 agora-esp32-devkit 的 `esp-vocat` 板级 profile。硬件资料见
[ESP-VoCat 开源硬件项目](https://oshwhub.com/esp-college/echoear)；硬件具备的器件与算法能力
不等于本固件已实现的功能。

## 硬件与当前 profile

| 项目 | 当前配置 |
| --- | --- |
| 主控与存储 | ESP32-S3、16 MB QIO Flash、80 MHz Octal PSRAM |
| 分区 | 根目录 `partitions/v2/16m.csv` |
| 音频 | ES7210 采集、ES8311 播放，16 kHz 单声道 signed 16-bit PCM |
| 公共 I2C0 | SDA GPIO2、SCL GPIO1；ES8311 `0x18`、ES7210 `0x40`、CST816S `0x15` |
| I2S | MCLK GPIO42、BCLK GPIO40、WS GPIO39、DOUT GPIO41 |
| 显示 | 360 × 360 ST77916 QSPI 圆屏，共享 LVGL UI |
| 输入 | CST816S 触摸中断 GPIO10、Boot GPIO0 |
| 控制台 | USB Serial/JTAG，避免 UART0 与板载外设引脚冲突 |

初始化时探测兼容的 PCB 版本：V1.0 使用 DIN GPIO15、PA GPIO4、GPIO3 低有效 LCD reset；
V1.2 使用 DIN GPIO3、PA GPIO15、GPIO47 高有效 LCD reset。无法探测 ES8311 时初始化失败，
不会盲选引脚。完整定义见 [board_config.h](board_config.h)、[board.cmake](board.cmake) 与
[sdkconfig.defaults](sdkconfig.defaults)。发布设备仍需确认实际 PSRAM 容量和 PCB 引脚映射。

## 构建

在工程根目录激活 ESP-IDF v5.5.2 后构建中英文固件：

```sh
. /path/to/esp-idf/export.sh
test "$(idf.py --version)" = "ESP-IDF v5.5.2"
python3 scripts/build_all.py --board esp-vocat --language both
```

中间构建目录为 `build/build-esp-vocat-<语言>/`，默认生成根目录 `releases/esp-vocat-zh-CN.bin` 与
`releases/esp-vocat-en-US.bin`。构建器默认清理所选构建/输出根目录，详情与烧录入口见
[根目录构建说明](../../../../README.zh-CN.md#构建)。本板不支持视频构建。

当前 UI 由板级配置自动启用，无需下载额外 UI 资源包。

## 使用与验证边界

短触屏幕或短按 Boot 开始/结束对话；长触或长按 3 秒进入配网。当前只向 SDK 提供主麦克风
采集通道，未接入第二麦克风、播放参考通道、本地 AEC、本地唤醒词或声源定位。

电量、IMU、PCB 电容控制、SD 卡、LED、摄像头扩展、关机与低功耗尚未接入。PCB V1.0 有固件
无法修复的已知硬件供电稳定性问题。本 profile 尚未完成真机验证，编译成功不能替代电源、
触摸、显示和双向音频验证。

板级集成约定见 [板级移植](../../../../docs/BOARD_PORTING.zh-CN.md)，
相关上游许可见 [MIT 许可文本](../../assets/LICENSE.xiaozhi-esp32)。
