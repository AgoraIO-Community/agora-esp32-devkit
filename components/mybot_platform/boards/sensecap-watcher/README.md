# SenseCAP Watcher

[English](README_en.md)

本说明对应 `agora-esp32-devkit` 的 `sensecap-watcher` profile。
当前实现以本目录的 [board.cmake](board.cmake)、[board_config.h](board_config.h) 和
[sdkconfig.defaults](sdkconfig.defaults) 为准。

## 当前配置

| 项目 | 配置 |
| --- | --- |
| MCU / 存储 | ESP32-S3；32 MB QIO Flash，Octal PSRAM、80 MHz |
| 音频 | ES8311 与 ES7243E，16 kHz 单声道 PCM |
| 音频 MCLK/BCLK/WS/DIN/DOUT | GPIO10/11/12/15/16 |
| 显示 | 412×412 SPD2010 QSPI，LVGL 状态界面 |
| 公共 I2C | SDA GPIO47、SCL GPIO48；TCA9555 地址 `0x21` |
| 输入 | 编码器 GPIO41/42；TCA9555 P0.3 编码器按键 |
| 分区 | [`partitions/v2/32m-sensecap.csv`](../../../../partitions/v2/32m-sensecap.csv) |

TCA9555 管理系统、屏幕和 codec 功放的上电时序。旋转编码器调节音量，短按开始或结束
对话，长按 3 秒进入配网。当前固件尚未接入摄像头、触摸、LED、电池状态、关机和低功耗。

## 构建

在工程根目录加载 ESP-IDF v5.5.2 环境后执行：

```sh
python3 scripts/build_all.py --board sensecap-watcher --language both
```

脚本通过 `MYBOT_BOARD` 选择本 profile，分别准备
`build/build-sensecap-watcher-zh-CN/` 和 `build/build-sensecap-watcher-en-US/`。
中文、英文固件使用各自的服务区域。构建脚本也会生成 `releases/sensecap-watcher-*.bin`
合并镜像；Watcher 应使用下述分段烧录流程，不能直接烧录这些合并镜像。

## 烧录并保留工厂数据

专用分区表保留从 `0x9000` 开始、长度 204800 字节的 `nvsfactory` 工厂分区。
首次烧录前，替换实际串口并备份该区域，将备份保存在设备之外并妥善保管：

```sh
python -m esptool --chip esp32s3 --port /dev/ttyUSB0 --baud 460800 \
  --before default_reset --after hard_reset --no-stub read_flash 0x9000 204800 nvsfactory.bin
```

对上述脚本生成的中文工程执行分段烧录并查看日志：

```sh
idf.py -B build/build-sensecap-watcher-zh-CN -p /dev/ttyUSB0 flash monitor
```

英文版本使用 `build/build-sensecap-watcher-en-US`。`idf.py flash` 按所选生成工程的
`flash_args` 写入 bootloader、分区表、OTA 数据和应用分段。应用分段从 `0x200000`
开始，不能把它写到 `0x0`。这些分段避开工厂分区；不要整片擦除，也不要用填充空洞的
合并镜像覆盖工厂数据。

完整环境及构建选项见[项目构建指南](../../../../README.zh-CN.md#构建)。实机验证状态见
[平台移植说明](../../../../docs/BOARD_PORTING.zh-CN.md)；编译成功不代表硬件功能已验证。
参考材料的 MIT 许可保留于 [LICENSE.xiaozhi-esp32](../../assets/LICENSE.xiaozhi-esp32)，
第三方归属见[许可说明](../../../../THIRD_PARTY_NOTICES.md)。
