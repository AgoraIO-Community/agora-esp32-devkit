# M5Stack AtomEchoS3R

本目录是 agora-esp32-devkit 的 `atom-echos3r` 板级 profile，面向
ESP32-S3-PICO-1-N8R8 与 ES8311 音频硬件。该 profile 无屏幕，不编入 LVGL。

## 硬件与当前 profile

| 项目 | 当前配置 |
| --- | --- |
| 存储 | 8 MB QIO Flash、80 MHz 8 MB Octal PSRAM |
| 分区 | 根目录 `partitions/v2/8m.csv` |
| ES8311 I2C0 | SDA GPIO45、SCL GPIO0；7 位地址 `0x18` |
| I2S 音频 | MCLK GPIO11、BCLK GPIO17、WS GPIO3、DIN GPIO4、DOUT GPIO48 |
| 扬声器功放 | GPIO18 使能 |
| 用户按键 | GPIO41，当前按低电平有效配置 |

音频接口使用 16 kHz、单声道、signed 16-bit PCM。引脚定义见
[board_config.h](board_config.h)，构建约束见 [board.cmake](board.cmake) 与
[sdkconfig.defaults](sdkconfig.defaults)。

## 构建

在工程根目录激活 ESP-IDF v5.5.2 后构建中英文固件：

```sh
. /path/to/esp-idf/export.sh
test "$(idf.py --version)" = "ESP-IDF v5.5.2"
python3 scripts/build_all.py --board atom-echos3r --language both
```

中间构建目录为 `build/build-atom-echos3r-<语言>/`，默认生成根目录 `releases/atom-echos3r-zh-CN.bin` 与
`releases/atom-echos3r-en-US.bin`。构建器默认清理所选构建/输出根目录，详情与烧录入口见
[根目录构建说明](../../../../README.zh-CN.md#构建)。本板不支持视频构建。

## 使用与验证边界

未保存 Wi-Fi 凭据时，设备创建配置热点并播放对应语言的配网提示音。短按 GPIO41 用户按键
开始或结束对话，长按 3 秒先停止 SDK，再进入配网。当前未接入本地唤醒词，不要求通过
语音唤醒操作设备。

GPIO41 用户按键映射、极性及 16 kHz 双向音频质量仍需真机验证。构建成功不代表已经完成
这些硬件验证；无屏状态可结合串口日志与语音提示确认。

板级集成约定见 [板级移植](../../../../docs/BOARD_PORTING.zh-CN.md)，
相关上游许可见 [MIT 许可文本](../../assets/LICENSE.xiaozhi-esp32)。
