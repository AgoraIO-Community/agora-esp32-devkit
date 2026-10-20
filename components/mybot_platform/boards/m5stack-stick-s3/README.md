# M5Stack StickS3

本目录是 agora-esp32-devkit 的 `m5stack-stick-s3` 板级 profile，提供 ES8311 音频、
ST7789P3 LCD、主按键与 M5PM1 电源初始化。

## 硬件与当前 profile

| 项目 | 当前配置 |
| --- | --- |
| 主控与存储 | ESP32-S3、8 MB QIO Flash、80 MHz 8 MB Octal PSRAM |
| 分区 | 根目录 `partitions/v2/8m.csv` |
| 公共 I2C0 | SDA GPIO47、SCL GPIO48；M5PM1 `0x6e`、ES8311 `0x18` |
| ES8311 I2S | MCLK GPIO18、BCLK GPIO17、WS GPIO15、DIN GPIO16、DOUT GPIO14 |
| LCD | 135 × 240 ST7789P3，显示偏移 (52, 40)，共享 LVGL UI |
| LCD SPI3 | MOSI GPIO39、SCLK GPIO40、CS GPIO41、DC GPIO45、RESET GPIO21、背光 GPIO38 |
| 输入 | 主按键 GPIO11，低电平有效 |

M5PM1 G2 为显示与 codec 供电，在运行期间保持开启；G3 仅在播放期间开启扬声器功放。
音频使用 16 kHz 单声道 signed 16-bit PCM。完整定义见 [board_config.h](board_config.h)、
[board.cmake](board.cmake) 与 [sdkconfig.defaults](sdkconfig.defaults)。

## 构建

在工程根目录激活 ESP-IDF v5.5.2 后构建中英文固件：

```sh
. /path/to/esp-idf/export.sh
test "$(idf.py --version)" = "ESP-IDF v5.5.2"
python3 scripts/build_all.py --board m5stack-stick-s3 --language both
```

中间构建目录为 `build/build-m5stack-stick-s3-<语言>/`，默认生成根目录 `releases/m5stack-stick-s3-zh-CN.bin` 与
`releases/m5stack-stick-s3-en-US.bin`。构建器默认清理所选构建/输出根目录，详情与烧录入口见
[根目录构建说明](../../../../README.zh-CN.md#构建)。本板不支持视频构建。

## 使用与验证边界

短按主按键开始/结束对话，长按 3 秒进入配网。GPIO12、IMU、红外、电池状态、关机手势及
低功耗尚未接入。硬件上存在这些器件不代表当前固件可使用它们。

本 profile 尚未完成真机验证，仍需核对电源时序、LCD 颜色和方向、按键、配网及 16 kHz
双向音频。编译成功不能替代上述硬件验证。

板级集成约定见 [板级移植](../../../../docs/BOARD_PORTING.zh-CN.md)，
相关上游许可见 [MIT 许可文本](../../assets/LICENSE.xiaozhi-esp32)。
