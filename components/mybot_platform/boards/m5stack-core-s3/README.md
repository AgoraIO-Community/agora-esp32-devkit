# M5Stack CoreS3

本目录是 agora-esp32-devkit 的 `m5stack-core-s3` 板级 profile，提供音频、触摸、像素眼镜
LVGL UI 与可选 GC0308 视频上行。

## 硬件与当前 profile

| 项目 | 当前配置 |
| --- | --- |
| 主控与存储 | ESP32-S3、16 MB QIO Flash、80 MHz Quad PSRAM |
| 分区 | 根目录 `partitions/v2/16m.csv` |
| 公共 I2C1 | SDA GPIO12、SCL GPIO11 |
| ES7210 / AW88298 I2S | MCLK GPIO0、BCLK GPIO34、WS GPIO33、DIN GPIO14、DOUT GPIO13 |
| 显示 | 320 × 240 ILI9342，SPI MOSI GPIO37、SCLK GPIO36、CS GPIO3、DC GPIO35 |
| 触摸与电源 | FT6336 `0x38`、AXP2101 `0x34`、AW9523 `0x58` |
| 视频 | GC0308，RTC 连接期间上传 320 × 240 JPEG，最多 1 fps |

GPIO0 是音频 MCLK，不能当作 Boot 用户按键。音频使用 16 kHz 单声道 signed 16-bit PCM，
启动时恢复持久化音量。完整定义见 [board_config.h](board_config.h)、[board.cmake](board.cmake)
与 [sdkconfig.defaults](sdkconfig.defaults)。

## 构建

在工程根目录激活 ESP-IDF v5.5.2 后构建中英文固件：

```sh
. /path/to/esp-idf/export.sh
test "$(idf.py --version)" = "ESP-IDF v5.5.2"
python3 scripts/build_all.py --board m5stack-core-s3 --language both
```

默认自动开启视频，中间构建目录为 `build/build-m5stack-core-s3-<语言>-video/`，生成根目录 `releases/m5stack-core-s3-zh-CN-video.bin` 与
`releases/m5stack-core-s3-en-US-video.bin`。需要无视频对照固件时运行：

```sh
python3 scripts/build_all.py --board m5stack-core-s3 --language both --video off
```

此时文件名为 `releases/m5stack-core-s3-zh-CN.bin` 与 `releases/m5stack-core-s3-en-US.bin`。
构建器默认清理所选构建/输出根目录；如需同时保留两组，使用独立输出目录，详见
[根目录构建说明](../../../../README.zh-CN.md#构建)。

## 使用与验证边界

短触屏幕开始/结束对话，长按屏幕 3 秒进入配网。当前 UI 使用固定工艺灰像素眼镜皮肤。
详细行为见 [CoreS3 UI](../../../../docs/CORES3_UI.zh-CN.md) 与
[CoreS3 视频](../../../../docs/CORES3_VIDEO.zh-CN.md)。

此前固件的配网、双向音频、摄像头上行与旧状态卡片 UI 已有真机结果；这些结果不能代表新
像素眼镜 UI 已完成真机回归。当前 UI、视频和音频仍需按本轮固件联合验收。摄像头预览、视频
下行、电池状态和自动休眠尚未接入。构建成功不能替代实机验证。

板级集成约定见 [板级移植](../../../../docs/BOARD_PORTING.zh-CN.md)，
相关上游许可见 [MIT 许可文本](../../assets/LICENSE.xiaozhi-esp32)。
