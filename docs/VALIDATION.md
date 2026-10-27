# Validation matrix / 验证矩阵

This is the single source of truth for current build coverage, real-device results and known
unsupported areas. Board READMEs describe hardware and operation only; the
[release checklist](RELEASING.md) describes required checks, not completed results.

本文档是构建覆盖、真机结果和已知未支持能力的唯一状态来源。板卡 README 只描述硬件与操作，
[发布检查清单](RELEASING.zh-CN.md)描述发布前必须执行的检查，不表示这些检查已经完成。

## Legend / 图例

- **CI build / CI 构建**: Both `zh-CN` and `en-US` are present in the
  [CI matrix](../.github/workflows/ci.yml). This is coverage unless recorded build evidence is
  explicitly listed.
- **Recorded build / 已记录构建**: The repository contains a specific successful
  `reconfigure/build/size/merge-bin` result for that profile.
- **Hardware / 真机**: Results recorded from physical devices. A successful build, host test or
  simulator preview does not satisfy this column.

## Matrix / 矩阵

| Board profile | Build evidence / 构建证据 | Hardware / 真机 | Unsupported or unverified / 未支持或待验证 |
| --- | --- | --- | --- |
| `atom-echos3r` | CI: zh-CN, en-US | Not completed / 未完成 | GPIO41 mapping and polarity, 16 kHz bidirectional audio; no display or video. |
| `bread-compact-wifi-lcd` | CI: zh-CN, en-US; recorded merged build / 已记录合并构建 | Not completed / 未完成 | Capacity, panel, backlight, audio and repeated provisioning; no touch, camera, volume keys, LED, lamp or power management. |
| `esp-vocat` | CI: zh-CN, en-US | Not completed / 未完成 | Both PCB revisions still require power, pin-map, audio and touch validation; no battery, IMU, slider, SD, LED, camera expansion, local AEC, reference audio, shutdown or low power. |
| `esp32-s3-touch-amoled-1.75` | CI: zh-CN, en-US | Not completed / 未完成 | Power, audio slots, display and touch; no video, playback reference, local AEC, RTC, IMU, TF card, battery reporting, sleep or shutdown. |
| `esp32-s3-touch-amoled-1.75c` | CI: zh-CN, en-US | Not completed / 未完成 | Same as 1.75, with no TCA9554 support; verify detected Flash capacity before expanding the partition layout. |
| `esp32-s3-touch-amoled-2.16` | CI: zh-CN, en-US; recorded merged build / 已记录合并构建 | Not completed / 未完成 | Physical Flash/PSRAM and TCA9554 presence, power, 480x480 display, touch and audio; no video, playback reference, local AEC, RTC, IMU, TF card, battery reporting, sleep or shutdown. |
| `esp32-s3-touch-amoled-1.8-v2` | CI: zh-CN, en-US; recorded merged build / 已记录合并构建 | Not completed / 未完成 | Physical Flash/PSRAM, AXP2101/TCA9554 sequencing, 368x448 QSPI display, CST816S and 16 kHz ES8311 audio require device validation; no AXP2101 power-key action, battery reporting, sleep, shutdown or video. |
| `esp32-s3-touch-lcd-1.85` | CI: zh-CN, en-US; recorded merged build / 已记录合并构建 | Not completed / 未完成 | Physical Flash/PSRAM, GPIO7 power hold, panel-version selection, backlight, CST816S and right-slot microphone/both-slot playback require device validation; GPIO6 power button, battery reporting, sleep, shutdown and video are not implemented. |
| `genjutech-s3-1.54tft` | CI: zh-CN, en-US; recorded merged build / 已记录合并构建 | Not completed / 未完成 | Physical Flash/PSRAM, ST7789, ES8311, PA/display coupling, Boot and volume inputs require device validation; no touch, LED, battery/charging, sleep, shutdown or video. |
| `m5stack-core-s3` | CI: zh-CN, en-US, video, light-theme and static-animation variants; recorded video build / 已记录视频构建 | Partial / 部分通过: earlier provisioning, bidirectional audio, camera uplink and status-card UI; current pixel UI and shared UI changes pending / 旧通路通过，当前像素 UI 与共享 UI 待回归 | Current pixel UI, capture scheduling and DMA regression; no video downlink, camera preview, battery reporting or automatic sleep. Video is opt-in for direct builds. |
| `m5stack-stick-s3` | CI: zh-CN, en-US | Not completed / 未完成 | Power, audio, input and panel; no GPIO12 support, IMU, infrared, battery reporting, shutdown or low power. |
| `respeaker-flex-xvf3800-circular4-xiao` | CI: zh-CN, en-US | Not completed / 未完成 | Requires separately flashed XVF3800 Circular-4 16 kHz two-channel I2S firmware; XVF version, clock direction, routing and audio pending. No Linear-4, XVF firmware update, LED ring, LCD or video. |
| `sensecap-watcher` | CI: zh-CN, en-US | Not completed / 未完成 | Power sequencing, codec 16 kHz mode, audio, encoder and display; no camera, touch, LED, battery reporting, shutdown or low power. Preserve the factory-data partition. |
| `xingzhi-cube-1.54tft-wifi` | CI: zh-CN, en-US; recorded merged build / 已记录合并构建 | Not completed / 未完成 | Physical Flash/PSRAM, GPIO21 power hold, backlight, display, buttons and 16 kHz audio; no touch, video, battery/charging, shutdown, sleep or low power. |
| `zhengchen-1.54tft-ml307` | CI: zh-CN, en-US | Not completed / 未完成 | ML307/4G is not implemented; Wi-Fi remains the network path. Audio, panel and buttons still need device validation; no battery status, temperature monitoring, sleep or low power. |
| `zhengchen-1.54tft-wifi` | CI: zh-CN, en-US | Not completed / 未完成 | Physical PSRAM capacity, audio, panel and buttons still need device validation; no modem/4G, battery/charging, temperature monitoring, sleep or low power. |
| `xingzhi-metal-1.54-wifi` | CI: zh-CN, en-US; local merged build | Not completed / 未完成 | PCA9557, ES8311, ST7789, touch and PA mapping require device validation; no battery, sleep or shutdown. |
| `xingzhi-cube-0.85tft-wifi` | CI: zh-CN, en-US; local merged build | Not completed / 未完成 | NV3023 panel, backlight, raw I2S and power require device validation; no touch or codec control. |
| `xingzhi-cube-0.96oled-wifi` | CI: zh-CN, en-US; local merged build | Not completed / 未完成 | Headless subset; SSD1306 monochrome UI is not implemented. Raw I2S and volume keys require device validation. |
| `xingzhi-abs-2.0` | CI: zh-CN, en-US; local merged build | Not completed / 未完成 | Wi-Fi subset only; SD, vibration, cellular, battery, PCA9557 power paths, sleep and shutdown are not implemented. |
| `m5stack-stopwatch` | CI: zh-CN, en-US; local merged build | Not completed / 未完成 | M5PM1/M5IOE1 sequencing, CO5300 466×466 panel, CST820B touch, buttons, audio and round safe area require device validation; battery reporting and motor are not implemented. |
| `esp32-s3-touch-amoled-1.8` | CI: zh-CN, en-US; local merged build | Not completed / 未完成 | TCA9554/AXP2101 sequencing, SH8601 368×448 timing and FT5x06 mapping require device validation. |
| `esp32-s3-touch-amoled-1.32` | CI: zh-CN, en-US; local merged build | Not completed / 未完成 | SH8601 466×466 timing, power rail, audio and panel offset require device validation; touch is not implemented. |
| `esp32-s3-touch-amoled-1.43c` | CI: zh-CN, en-US; local merged build | Not completed / 未完成 | SH8601 466×466 timing, ES8311/ES7210 routing and power require device validation; touch is not implemented. |
| `esp32-s3-touch-amoled-2.06` | CI: zh-CN, en-US; local merged build | Not completed / 未完成 | SH8601 410×502 timing, 22-pixel X gap, AXP2101, FT5x06 and dual-codec routing require device validation. |
| `atoms3r-echo-base` | CI: zh-CN, en-US; local merged build | Not completed / 未完成 | Display/audio mapping requires validation; companion IO/power and LP5562 backlight integration are not implemented. |
| `atoms3r-echo-pyramid` | CI: zh-CN, en-US; local merged build | Not completed / 未完成 | Display/dual-codec mapping requires validation; companion IO/power and LP5562 backlight integration are not implemented. |
| `sp-esp32-s3-1.28-box` | CI: zh-CN, en-US; local merged build | Not completed / 未完成 | GC9A01 color/orientation, touch, backlight and power require device validation; battery/charging reporting is not implemented. |
| `esp-box` | CI: zh-CN, en-US; local merged build | Not completed / 未完成 | ILI9341 initialization, audio routing, touch and board PMIC/power sequencing require device validation. |
| `esp-box-3` | CI: zh-CN, en-US; local merged build | Not completed / 未完成 | ILI9341 initialization, audio routing, touch and board PMIC/power sequencing require device validation. |
| `esp-box-lite` | CI: zh-CN, en-US; local merged build | Not completed / 未完成 | Codec assignment, ILI9341, power sequencing and ADC buttons require validation; ADC-button support is not implemented. |
| `esp32s3-korvo2-v3` | CI: zh-CN, en-US; local merged build | Not completed / 未完成 | TCA9554 reset/CS, ILI9341, dual-codec routing and board power require device validation; ADC keys, camera and battery are not implemented. |
| `lichuang-dev` | CI: zh-CN, en-US; local merged build | Not completed / 未完成 | PCA9557 display/PA control, FT5x06, ST7789 and dual-codec routing require device validation; camera and battery are not implemented. |
| `quandong-s3-dev` | CI: zh-CN, en-US; local merged build | Not completed / 未完成 | PCA9557 output mapping, active-low PA, ILI9341 and audio routing require device validation. |
| `rymcu-bigsmart` | CI: zh-CN, en-US; local merged build | Not completed / 未完成 | PCA9557 display/PA control, GT911, ST7789 and dual-codec routing require device validation; camera and battery are not implemented. |
| `df-k10` | CI: zh-CN, en-US; local merged build | Not completed / 未完成 | ILI9341 and dual-codec mapping require validation; TCA9555 keys, camera, LED ring and board power paths are not implemented. |
| `mixgo-nova` | CI: zh-CN, en-US; local merged build | Not completed / 未完成 | ES8374/ST7789 mapping, power, audio and 128×160 UI require device validation; RGB LED is not implemented. |
| `taiji-pi-s3` | CI: zh-CN, en-US; local merged build | Not completed / 未完成 | ST77916, CST816S, PDM capture, I2S playback, mute/PA and round UI require device validation; board power sequencing is not complete. |
| `freenove-esp32s3-display-2.8-lcd` | CI: zh-CN, en-US; local merged build | Not completed / 未完成 | ILI9341, ES8311, PA and backlight require device validation; camera, LED and battery paths are not implemented. |
| `esp32-s3-touch-lcd-1.54` | CI: zh-CN, en-US; local merged build | Not completed / 未完成 | ST7789, CST816S, dual codec, auxiliary power and three keys require device validation; battery management, sleep and shutdown are not implemented. |
| `esp32-s3-touch-lcd-1.83` | CI: zh-CN, en-US; local merged build | Not completed / 未完成 | ST7789, CST816S, AXP2101 and dual codec require device validation; PWR41, battery reporting, sleep and shutdown are not implemented. |
| `esp32-s3-lcd-0.85` | CI: zh-CN, en-US; local merged build | Not completed / 未完成 | ST7789 128×128 layout, dual codec, battery enable and keys require device validation; touch is not implemented. |
| `esp32-s3-touch-lcd-1.85b` | CI: zh-CN, en-US; local merged build | Not completed / 未完成 | ST77916 panel revisions, CST816S-compatible touch, dual codec, reset/backlight and power require device validation. |
| `esp32-s3-touch-lcd-1.85c` | CI: zh-CN, en-US; local merged build | Not completed / 未完成 | ST77916, TCA9554 reset, CST816S-compatible touch, dual codec V2 path and power require device validation; V1 raw-I2S is not implemented. |
| `sp-esp32-s3-1.54-muma` | CI: zh-CN, en-US; local merged build | Not completed / 未完成 | ST7789 vendor tuning, CST816S, ES8311, battery and power hold require device validation; sleep and shutdown are not implemented. |
| `atk-dnesp32s3-box0` | CI: zh-CN, en-US; local merged build | Not completed / 未完成 | ST7789, ES8311, GPIO2/GPIO14 power sequencing, three keys and battery require device validation; sleep, shutdown and charging are not implemented. |
| `nulllab-ai-vox-v3` | CI: zh-CN, en-US; local merged build | Not completed / 未完成 | Wi-Fi subset; ST7789, ES8311, battery and keys require device validation. ML307 networking is not implemented. |
| `aipi-lite` | CI: zh-CN, en-US; local merged build | Not completed / 未完成 | ST7789 128×128, ES8311, power hold and battery require device validation; power key, sleep and shutdown are not implemented. |

## Global boundaries / 全局边界

- Local wake words are not implemented.
- ML307/4G networking is not implemented; all profiles use Wi-Fi.
- Only `m5stack-core-s3` currently implements encoded video uplink.
- NVS encryption, Flash encryption and Secure Boot belong to product provisioning rather than
  these default firmware profiles.

状态变化时只更新本矩阵；专题文档和板卡 README 应链接到这里，不再复制验证状态。
