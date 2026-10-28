# Board documentation / 板卡说明

These documents describe the board profiles implemented by `mybot-esp32`. Pins, power sequencing,
drivers and build constraints are defined by the checked-in `board.cmake`, `board_config.h` and
`sdkconfig.defaults` files. Current validation status is centralized in
[`docs/VALIDATION.md`](../../../docs/VALIDATION.md).

以下说明对应本工程已实现的 72 个板型，其中 64 个带屏、8 个无屏。硬件配置、输入行为和
构建约束以当前板级代码为准；验证状态统一见
[验证矩阵](../../../docs/VALIDATION.md)。不同尺寸、芯片或硬件版本的板卡不能仅凭外观
共用固件。

| Board profile | 板卡说明 | 显示 | 视频 |
| --- | --- | --- | --- |
| `atom-echos3r` | [README](atom-echos3r/README.md) | 无屏幕 | 无 |
| `bread-compact-wifi-lcd` | [README](bread-compact-wifi-lcd/README.md) | 240×320 ST7789 | 无 |
| `esp-vocat` | [README](esp-vocat/README.md) | ST77916 触摸屏 | 无 |
| `esp32-s3-touch-amoled-1.75` | [README](esp32-s3-touch-amoled-1.75/README.md) | CO5300 AMOLED 触摸屏 | 无 |
| `esp32-s3-touch-amoled-1.75c` | [README](esp32-s3-touch-amoled-1.75c/README.md) | CO5300 AMOLED 触摸屏 | 无 |
| `esp32-s3-touch-amoled-2.16` | [README](esp32-s3-touch-amoled-2.16/README.md) | 480×480 CO5300 AMOLED 触摸屏 | 无 |
| `esp32-s3-touch-amoled-1.8-v2` | [README](esp32-s3-touch-amoled-1.8-v2/README.md) | 368×448 CO5300 AMOLED 触摸屏 | 无 |
| `esp32-s3-touch-lcd-1.85` | [README](esp32-s3-touch-lcd-1.85/README.md) | 360×360 ST77916 触摸屏 | 无 |
| `genjutech-s3-1.54tft` | [README](genjutech-s3-1.54tft/README.md) | 240×240 ST7789 | 无 |
| `m5stack-core-s3` | [README](m5stack-core-s3/README.md) | ILI9342 触摸屏 | GC0308，上传 1 fps |
| `m5stack-stick-s3` | [README](m5stack-stick-s3/README.md) | 135×240 ST7789P3 | 无 |
| `respeaker-flex-xvf3800-circular4-xiao` | [README](respeaker-flex-xvf3800-circular4-xiao/README.md) | 无屏幕 | 无 |
| `sensecap-watcher` | [中文](sensecap-watcher/README.md) / [English](sensecap-watcher/README_en.md) | SPD2010 | 无 |
| `xingzhi-cube-1.54tft-wifi` | [README](xingzhi-cube-1.54tft-wifi/README.md) | 240×240 ST7789 | 无 |
| `zhengchen-1.54tft-ml307` | [README](zhengchen-1.54tft-ml307/README.md) | ST7789 | 无 |
| `zhengchen-1.54tft-wifi` | [README](zhengchen-1.54tft-wifi/README.md) | ST7789 | 无 |

### Additional ESP32-S3 profiles / 新增板型

These profiles reuse the shared board lifecycle in `boards/common/`. Their `board_config.h` and
`board.cmake` files are the authoritative pin and capability records. None has completed
real-device validation.

这些板型复用 `boards/common/` 的共享生命周期；引脚与能力以各自的 `board_config.h` 和
`board.cmake` 为准。以下板型均未完成真机验证。

| Board profile | Display / 显示 | Audio / 音频 | Input / 输入 | Reduced scope / 当前子集 |
| --- | --- | --- | --- | --- |
| [xingzhi-metal-1.54-wifi](xingzhi-metal-1.54-wifi/README.md) | 240×240 ST7789 | ES8311 | PCA9557 touch + Boot | Touch, PCA9557 power/reset and PA require device validation. |
| [xingzhi-cube-0.85tft-wifi](xingzhi-cube-0.85tft-wifi/README.md) | 128×128 NV3023 | Raw I2S | Boot | No touch or codec control. |
| [xingzhi-cube-0.96oled-wifi](xingzhi-cube-0.96oled-wifi/README.md) | Headless | Raw I2S | Boot + two volume keys | SSD1306 monochrome UI is not implemented. |
| [xingzhi-abs-2.0](xingzhi-abs-2.0/README.md) | 240×240 ST7789 | ES8311 | Boot + two volume keys | Wi-Fi only; SD, vibration and cellular are not implemented. |
| [m5stack-stopwatch](m5stack-stopwatch/README.md) | 466×466 round CO5300 | ES8311 | CST820B-compatible CST816S path + two buttons | M5PM1/M5IOE1 power sequencing and round safe area are implemented; battery reporting, motor, sleep and shutdown are not. |
| [esp32-s3-touch-amoled-1.8](esp32-s3-touch-amoled-1.8/README.md) | 368×448 SH8601 QSPI | ES8311 | FT5x06 + Boot | Device timing and touch mapping require validation. |
| [esp32-s3-touch-amoled-1.32](esp32-s3-touch-amoled-1.32/README.md) | 466×466 SH8601 QSPI | ES8311 | Boot | Touchless profile. |
| [esp32-s3-touch-amoled-1.43c](esp32-s3-touch-amoled-1.43c/README.md) | 466×466 SH8601 QSPI | ES8311 + ES7210 | Boot | Touchless profile. |
| [esp32-s3-touch-amoled-2.06](esp32-s3-touch-amoled-2.06/README.md) | 410×502 SH8601 QSPI | ES8311 + ES7210 | FT5x06 + Boot | 22-pixel X gap; device timing requires validation. |
| [atoms3r-echo-base](atoms3r-echo-base/README.md) | 128×128 GC9A01-class SPI panel | ES8311 | Boot | Echo-base IO/power and LP5562 backlight integration are not implemented. |
| [atoms3r-echo-pyramid](atoms3r-echo-pyramid/README.md) | 128×128 GC9A01-class SPI panel | ES8311 + ES7210 | Boot | Echo-pyramid IO/power and LP5562 backlight integration are not implemented. |
| [sp-esp32-s3-1.28-box](sp-esp32-s3-1.28-box/README.md) | 240×240 round GC9A01 | ES8311 | CST816S-compatible touch + Boot | Battery/charging reporting and panel vendor tuning require validation. |
| [esp-box](esp-box/README.md) | 320×240 ILI9341 | ES8311 + ES7210 | Boot | Touch, board PMIC options and power sequencing require validation. |
| [esp-box-3](esp-box-3/README.md) | 320×240 ILI9341 | ES8311 + ES7210 | Boot | Touch, board PMIC options and power sequencing require validation. |
| [esp-box-lite](esp-box-lite/README.md) | 320×240 ILI9341 | ES8311 + ES7210 profile | Boot | ADC buttons, codec assignment and power sequencing require validation. |
| [esp32s3-korvo2-v3](esp32s3-korvo2-v3/README.md) | 320×240 ILI9341 | ES8311 + ES7210 | Boot | TCA9554 reset/CS is implemented; ADC keys, camera, battery and sleep are not. |
| [lichuang-dev](lichuang-dev/README.md) | 320×240 ST7789 | ES8311 + ES7210 | FT5x06 + Boot | PCA9557 display/PA control is implemented; camera and battery are not. |
| [quandong-s3-dev](quandong-s3-dev/README.md) | 320×240 ILI9341 | ES8311 | Boot | PCA9557 setup and active-low PA are implemented; hardware mapping requires validation. |
| [rymcu-bigsmart](rymcu-bigsmart/README.md) | 320×240 ST7789 | ES8311 + ES7210 | GT911 + Boot | PCA9557 display/PA control is implemented; camera and battery are not. |
| [df-k10](df-k10/README.md) | 240×320 ILI9341 | ES8311 + ES7210 | Boot | TCA9555 keys, camera, LED ring and power paths are not implemented. |
| [mixgo-nova](mixgo-nova/README.md) | 128×160 ST7789 | ES8374 | Boot | RGB LED and hardware validation are pending. |
| [taiji-pi-s3](taiji-pi-s3/README.md) | 360×360 round ST77916 QSPI | ES8311 playback + PDM capture | CST816S + Boot | PDM capture and mute/PA lifecycle are implemented; board power sequencing requires validation. |
| [freenove-esp32s3-display-2.8-lcd](freenove-esp32s3-display-2.8-lcd/README.md) | 320×240 ILI9341 | ES8311 | Boot | Camera, LED and battery paths are not implemented. |
| [esp32-s3-touch-lcd-1.54](esp32-s3-touch-lcd-1.54/README.md) | 240×240 ST7789 | ES8311 + ES7210 | CST816S + three buttons | Battery management, sleep and shutdown are not implemented. |
| [esp32-s3-touch-lcd-1.83](esp32-s3-touch-lcd-1.83/README.md) | 240×284 ST7789 | ES8311 + ES7210 | CST816S + Boot | AXP2101, PWR41, battery reporting and sleep require device validation. |
| [esp32-s3-lcd-0.85](esp32-s3-lcd-0.85/README.md) | 128×128 ST7789 | ES8311 + ES7210 | three buttons | Touchless profile; battery timing and 8 MB layout require validation. |
| [esp32-s3-touch-lcd-1.85b](esp32-s3-touch-lcd-1.85b/README.md) | 360×360 ST77916 QSPI | ES8311 + ES7210 | CST816S-compatible touch + Boot | Vendor panel revisions and touch mapping require validation. |
| [esp32-s3-touch-lcd-1.85c](esp32-s3-touch-lcd-1.85c/README.md) | 360×360 ST77916 QSPI | ES8311 + ES7210 V2 path | CST816S-compatible touch + Boot; TCA9554 reset | V1 raw-I2S path and touch mapping require validation. |
| [sp-esp32-s3-1.54-muma](sp-esp32-s3-1.54-muma/README.md) | 240×240 ST7789 | ES8311 | CST816S + Boot | Vendor panel tuning, battery and power hold require validation. |
| [atk-dnesp32s3-box0](atk-dnesp32s3-box0/README.md) | 240×240 ST7789 | ES8311 | three buttons | Sleep, shutdown, charging and battery reporting are not implemented. |
| [nulllab-ai-vox-v3](nulllab-ai-vox-v3/README.md) | 240×240 ST7789 | ES8311 | Boot + two volume keys | Wi-Fi subset only; ML307 networking is not implemented. |
| [aipi-lite](aipi-lite/README.md) | 128×128 ST7789 | ES8311 | Boot | Power key, sleep and battery reporting are not implemented. |
| [magiclick-2p4](magiclick-2p4/README.md) | 128×128 NV3023 | ES8311 | three buttons | Battery, LED, sleep and shutdown are not implemented. |
| [movecall-cuican-esp32s3](movecall-cuican-esp32s3/README.md) | 240×240 GC9A01 | ES8311 | Boot | 8 MB layout requires validation; LED, battery, sleep and shutdown are not implemented. |
| [movecall-moji-esp32s3](movecall-moji-esp32s3/README.md) | 240×240 GC9A01 | ES8311 | Boot | LED, battery, sleep and shutdown are not implemented. |
| [doit-s3-aibox](doit-s3-aibox/README.md) | Headless | PDM capture + I2S playback | three buttons | PDM timing, LED, sleep and shutdown require validation. |
| [bread-compact-wifi](bread-compact-wifi/README.md) | Headless | separate raw I2S | Boot + two volume keys | OLED UI, GPIO47 key, lamp and LED are not implemented. |
| [hu-087](hu-087/README.md) | Headless | separate raw I2S | one button | OLED UI is not implemented; long press is provision instead of volume. |
| [kevin-sp-v4-dev](kevin-sp-v4-dev/README.md) | 240×280 ST7789 | ES8311 | Boot | Camera, LED, battery, sleep and shutdown are not implemented. |
| [du-chatx](du-chatx/README.md) | 128×160 ST7789 | separate raw I2S | Boot | GPIO1 power hold is implemented; battery, charging, LED, sleep and shutdown are not. |
| [minsi-k08-dual](minsi-k08-dual/README.md) | 240×240 ST7789 | separate raw I2S | Boot + two volume keys | Wi-Fi only; lamp, LED, battery, sleep and shutdown are not implemented. |
| [kevin-box-2](kevin-box-2/README.md) | Headless | ES8311 + ES7210 | Boot + two volume keys | SSD1306 UI, ML307, LED, battery, sleep and shutdown are not implemented. |
| [magiclick-2p5](magiclick-2p5/README.md) | 128×128 GC9107 or ST7735 | ES8311 | three buttons | Wi-Fi only; battery, LED, sleep and shutdown are not implemented. |
| [atk-dnesp32s3-box3](atk-dnesp32s3-box3/README.md) | 320×240 ST7789 | ES8311 + ES7210 | Boot | AW9523B power/PA/backlight implemented; K1/K2, camera, battery, sleep and shutdown are not. |
| [zhengchen-cam](zhengchen-cam/README.md) | 320×240 ST7789 | ES8311 output + ES7210 capture | Boot + two volume buttons | Camera, portrait mode, battery, temperature, sleep and shutdown are not implemented; PCA9557 PA and audio require validation. |
| [labplus-ledong-v2](labplus-ledong-v2/README.md) | 320×172 ST7789-compatible | ES8388 | Boot | STM8 display reset is implemented; touch, sensors, buzzer, RGB LED, motor, camera, sleep and shutdown are not. |
| [labplus-mpython-v3](labplus-mpython-v3/README.md) | 320×172 ST7789-compatible | ES8388 | Boot | Touch, sensors, buzzer, RGB LED, motor, camera, sleep and shutdown are not implemented. |
| [atk-dnesp32s3](atk-dnesp32s3/README.md) | 320×240 ST7789 | ES8388 | Boot | TCA9555 display power is implemented; camera, expander keys, dimming, battery, sleep and shutdown are not. |
| [yunliao-s3](yunliao-s3/README.md) | 320×240 ST7789 | ES8388 | Boot | Wi-Fi subset; ML307/4G, battery, USB and Bluetooth status paths are not implemented. |
| [lilygo-t-cameraplus-s3](lilygo-t-cameraplus-s3/README.md) | 240×240 ST7789 | MSM261 + MAX98357A | CST816D + Boot | V1.0/V1.1 only; camera, PMIC, KEY1, SD, IR, sleep and shutdown are not implemented. |
| [lilygo-t-cameraplus-s3-v1.2](lilygo-t-cameraplus-s3-v1.2/README.md) | 240×240 ST7789 | PDM microphone + MAX98357A | CST816D + Boot | V1.2 only; camera, PMIC, KEY1, SD, IR, sleep and shutdown are not implemented. |
| [lilygo-t-circle-s3](lilygo-t-circle-s3/README.md) | 160×160 round GC9D01N | MSM261 + MAX98357A | CST816D + Boot | Rotary encoder, APA102, battery, sleep and shutdown are not implemented. |
| [jiuchuan-s3](jiuchuan-s3/README.md) | 240×296 GC9309NA | ES8311 | Boot + volume buttons | Power-key actions, battery, charging, sleep and shutdown are not implemented. |
| [lilygo-t-display-s3-pro-mvsrlora](lilygo-t-display-s3-pro-mvsrlora/README.md) | 222×480 ST7796 | PDM capture + MAX98357A playback | CST226SE + Boot | SY6970 setup is implemented; SD, LoRa, vibration, RTC, IMU, battery and sleep are not. |
| [atk-dnesp32s3-box](atk-dnesp32s3-box/README.md) | 320×240 i80 ST7789 | ES8311 | Boot | XL9555/TCA9555 power setup is implemented; expander keys, camera, battery, dimming, sleep and shutdown are not. |
| [tudouzi](tudouzi/README.md) | Headless | ES8311 + ES7210 | Boot + two volume keys | Wi-Fi-only subset; SSD1306 UI, ML307, LED, battery, sleep and shutdown are not implemented. |

## Build, flash and validation / 构建、烧录与验证

Activate ESP-IDF v5.5.2 from the repository root:

```sh
. /path/to/esp-idf/export.sh
test "$(idf.py --version)" = "ESP-IDF v5.5.2"
```

Build both languages for one profile:

```sh
python3 scripts/build_all.py --board <board-id> --language both
```

The script writes managed builds to `build/` and merged images to `releases/`, derives display
and video capability from the profile, uses the bundled 60 ms RTSA cadence and enables CoreS3
video by default. Use `--video off` for a no-video comparison. See the
[project README](../../../README.md#build) for root ownership and cleanup rules.

For a direct build and flash, use a dedicated build directory and port:

```sh
idf.py -B build/<board-id> \
  -DMYBOT_BOARD=<board-id> \
  -DSDKCONFIG=build/<board-id>/sdkconfig \
  -DSDKCONFIG_DEFAULTS="sdkconfig.defaults;ci/ptime60.defaults" build
idf.py -B build/<board-id> -p PORT flash monitor
```

Add `ci/en-us.defaults` for English. Add `ci/video.defaults` only when building CoreS3 with
video. SenseCAP Watcher requires the factory-partition backup procedure in its board README.

Board input, provisioning and hardware-specific prerequisites remain in each board README.
Completed build and hardware results are tracked only in the
[validation matrix](../../../docs/VALIDATION.md).

设备首次启动无 Wi-Fi 凭据时自动配网；长按配网输入会先停止 mybot，再进入配网流程，
联网后重新启动 mybot。各板的配网输入、短按/触摸和音量输入见对应 README。
