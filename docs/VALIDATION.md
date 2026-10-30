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
- **Hardware / 真机**: Results recorded from physical devices. A successful build or host test
  does not satisfy this column.

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
| `genjutech-s3-1.54tft` | CI: zh-CN, en-US; 2026-10-29 ESP-IDF v6.1 zh-CN and en-US recorded merged builds / v6.1 中英文已记录合并构建 | Not completed / 未完成 | Physical Flash/PSRAM, ST7789, ES8311, PA/display coupling, Boot and volume inputs require device validation; no touch, LED, battery/charging, sleep, shutdown or video. |
| `m5stack-core-s3` | CI: zh-CN, en-US, video, light-theme and static-animation variants; 2026-10-29 ESP-IDF v6.1 video build recorded / v6.1 视频构建已记录 | Partial / 部分通过: earlier provisioning, bidirectional audio, camera uplink and status-card UI; 2026-10-29 device log confirms camera stream startup with the PSRAM UI stack / UI 栈迁入 PSRAM 后摄像头启动通过 | Current pixel UI, capture cadence, frame upload and repeated start/stop regression; no video downlink, camera preview, battery reporting or automatic sleep. Video is opt-in for direct builds. |
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
| `magiclick-2p4` | CI: zh-CN, en-US; local merged build | Not completed / 未完成 | NV3023, ES8311, PA, keys and power require device validation; battery, LED, sleep and shutdown are not implemented. |
| `movecall-cuican-esp32s3` | CI: zh-CN, en-US; local merged build | Not completed / 未完成 | GC9A01, ES8311, PA and 8 MB layout require device validation; LED, battery, sleep and shutdown are not implemented. |
| `movecall-moji-esp32s3` | CI: zh-CN, en-US; local merged build | Not completed / 未完成 | GC9A01, ES8311, PA, keys and power require device validation; LED, battery, sleep and shutdown are not implemented. |
| `doit-s3-aibox` | CI: zh-CN, en-US; local merged build | Not completed / 未完成 | Headless PDM capture, I2S playback, GPIO48 power and keys require device validation; LED, sleep and shutdown are not implemented. |
| `bread-compact-wifi` | CI: zh-CN, en-US; local merged build | Not completed / 未完成 | Headless raw I2S, Boot/volume keys and storage require validation; OLED UI, GPIO47 key, lamp and LED are not implemented. |
| `hu-087` | CI: zh-CN, en-US; local merged build | Not completed / 未完成 | Headless raw I2S, GPIO17 amplifier enable and GPIO18 input require validation; SSD1306 UI is not implemented and long press is used for provisioning. |
| `kevin-sp-v4-dev` | CI: zh-CN, en-US; local merged build | Not completed / 未完成 | Physical Flash/PSRAM, ST7789 mode 3 timing, ES8311 on I2C1, PA and Boot input require device validation; camera, LED, battery, sleep and shutdown are not implemented. |
| `du-chatx` | CI: zh-CN, en-US; local merged build | Not completed / 未完成 | Physical Flash/PSRAM, GPIO1 power hold, ST7789 orientation, raw audio slots and Boot input require device validation; battery, charging, LED, sleep and shutdown are not implemented. |
| `minsi-k08-dual` | CI: zh-CN, en-US; local merged build | Not completed / 未完成 | Physical Flash/PSRAM, ST7789 orientation, raw audio slots and three keys require device validation; Wi-Fi-only subset has no ML307, battery, LED, lamp, sleep or shutdown. |
| `kevin-box-2` | CI: zh-CN, en-US; local merged build | Not completed / 未完成 | Headless subset; AXP2101, ES8311/ES7210 routing and keys require validation. SSD1306 UI, ML307, LED, battery, sleep and shutdown are not implemented. |
| `magiclick-2p5` | CI: zh-CN, en-US; local merged build | Not completed / 未完成 | GC9107/ST7735 ADC selection, ES8311, PA, keys and power require validation; Wi-Fi-only subset has no battery, LED, sleep or shutdown. |
| `atk-dnesp32s3-box3` | CI: zh-CN, en-US; local merged build | Not completed / 未完成 | AW9523B power/backlight/PA sequencing, ST7789 endian/orientation and dual codec require validation; K1/K2, camera, battery, sleep and shutdown are not implemented. |
| `zhengchen-cam` | CI: zh-CN, en-US; local merged build | Not completed / 未完成 | PCA9557 PA routing, 320×240 ST7789 orientation, ES8311/ES7210 slots and buttons require device validation; camera, portrait mode, battery, temperature, sleep and shutdown are not implemented. |
| `labplus-ledong-v2` | CI: zh-CN, en-US; local merged build | Not completed / 未完成 | STM8 display reset, 320×172 panel endian/orientation, ES8388, backlight and Boot input require device validation; touch and board peripherals are not implemented. |
| `labplus-mpython-v3` | CI: zh-CN, en-US; local merged build | Not completed / 未完成 | 320×172 panel endian/orientation, GPIO34 CS, GPIO33 backlight, ES8388 and Boot input require device validation; touch and board peripherals are not implemented. |
| `atk-dnesp32s3` | CI: zh-CN, en-US; local merged build | Not completed / 未完成 | TCA9555 LCD-power mapping, 320×240 ST7789, ES8388 and Boot input require device validation; camera, expander keys and backlight dimming are not implemented. |
| `yunliao-s3` | CI: zh-CN, en-US; local merged build | Not completed / 未完成 | ES8388, PA, 320×240 ST7789 and Boot input require device validation; Wi-Fi-only subset has no ML307/4G, battery, USB or Bluetooth status support. |
| `lilygo-t-cameraplus-s3` | CI: zh-CN, en-US; local merged build | Not completed / 未完成 | V1.0/V1.1 pin map, CST816D, ST7789, standard I2S microphone/speaker and backlight require device validation; camera and PMIC features are not implemented. |
| `lilygo-t-cameraplus-s3-v1.2` | CI: zh-CN, en-US; local merged build | Not completed / 未完成 | V1.2 pin map, CST816D, ST7789, PDM microphone, speaker enable and backlight require device validation; camera and PMIC features are not implemented. |
| `lilygo-t-circle-s3` | CI: zh-CN, en-US; local merged build | Not completed / 未完成 | GC9D01N initialization, 160×160 round orientation, CST816D, dual I2S audio and speaker enable require device validation; rotary encoder and APA102 are not implemented. |
| `jiuchuan-s3` | CI: zh-CN, en-US; local merged build | Not completed / 未完成 | GC9309NA initialization, 240×296 orientation, ES8311, PA, GPIO5 power hold and three keys require device validation; power-key actions and battery reporting are not implemented. |
| `lilygo-t-display-s3-pro-mvsrlora` | CI: zh-CN, en-US; recorded merged build | Not completed / 未完成 | ST7796 timing and orientation, CST226SE mapping, SY6970 sequencing, PDM capture, speaker enable and backlight require device validation; SD, LoRa, vibration, RTC, IMU, battery and sleep are not implemented. |
| `atk-dnesp32s3-box` | CI: zh-CN, en-US; recorded merged build | Not completed / 未完成 | i80 ST7789 timing and orientation, ES8311 routing and XL9555/TCA9555 power sequencing require device validation; expander keys, camera, battery, dimming, sleep and shutdown are not implemented. The NS4168-only hardware variant is not supported. |
| `tudouzi` | CI: zh-CN, en-US; recorded merged build | Not completed / 未完成 | Headless Wi-Fi subset; AXP2101 sequencing, ES8311/ES7210 routing, PA and keys require device validation. SSD1306 UI, ML307/4G, LED, battery reporting, sleep and shutdown are not implemented. |

## CoreS3 memory regression evidence / CoreS3 内存回归证据

On 2026-10-29, local ESP-IDF v5.5.2 `reconfigure/build/size` passed for CoreS3 video in
`zh-CN` and `en-US`. CoreS3 and ST7789 host fault-injection tests passed normally and with
UBSan; ASan could not run because even a minimal program failed at runtime startup on this host.
Formatting, SPDX and whitespace checks passed.

A user-provided real-device log for the earlier UI-stack-only change confirms RTC connected, `dma_buffer_max=8192`,
`dma_free=15703`, `dma_largest=11776`, DVP initialization and `stream_on` after moving the
7 KiB UI stack to PSRAM. Before this change, the largest DMA block was 5120 bytes and the
7680-byte allocation failed. This confirms one successful stream startup; frame capture,
SDK acceptance, server reception, audio/UI continuity, repeated start/stop and provisioning or
volume-persistence NVS writes still require validation with the current changes.

2026-10-29，本地 CoreS3 视频中英文版本通过 ESP-IDF v5.5.2 的 `reconfigure/build/size`。
CoreS3 与 ST7789 Host 故障注入测试的普通和 UBSan 版本通过；本机连最小程序也在 ASan
运行时启动阶段失败，因此未完成 ASan 验证。格式、SPDX 和空白检查通过。
用户提供的早期 UI 栈单项迁移固件真机日志确认，7 KiB UI 栈迁入 PSRAM 后，RTC 已连接，DMA 最大连续块从
5120 字节增至 11776 字节，DVP 初始化和 `stream_on` 成功。该结果仅证明一次启动通过；
当前改动下的帧采集、SDK 接受、服务端接收、音频/UI 连续性、重复启停，以及配网和音量
持久化 NVS 写入仍待验证。

## Platform PSRAM migration / 平台 PSRAM 迁移

The later platform-wide migration is described in [PSRAM_MEMORY.md](PSRAM_MEMORY.md). On
2026-10-29, CoreS3 video `zh-CN`/`en-US` and the CoreS3 offline audio test with resource monitoring
passed complete ESP-IDF v5.5.2 `reconfigure/build/size` checks. The linked CoreS3 `View` allocation
moved from 2492 internal BSS bytes to a 4-byte pointer. This does not measure runtime heap savings.

Eleven other profiles passed fresh configuration and compilation of 190 application/platform
translation units using their generated compile commands: `m5stack-stick-s3`, `esp-vocat`,
`esp32-s3-touch-amoled-1.75`, `esp32-s3-touch-amoled-1.75c`, `sensecap-watcher`,
`respeaker-flex-xvf3800-circular4-xiao`, `esp32-s3-touch-lcd-1.85`,
`lilygo-t-display-s3-pro-mvsrlora`, `mixgo-nova`, `atom-echos3r` and `zhengchen-1.54tft-wifi`.
These eleven checks did not link or merge firmware. Their PSRAM policy, external-stack and NVS
cache symbols were enabled; the headless profile omitted the LVGL runtime.

Audio/video, input and display host fault-injection suites passed, including standalone UBSan.
Coverage includes allocation/initialization failures, partial cleanup, completion before task
suspension, timeout with retained resources, callback ordering and repeated start/stop.
Formatting, SPDX and whitespace checks passed; ASan remains unavailable on this host.

The platform-wide migration has no new real-device result. The earlier stream-start log above
does not validate the new audio/video/input stacks, NVS cache or allocation lifetimes. Validate
full-duplex audio/video/UI, HTTPS, provisioning, persistence, repeated lifecycle operations and
internal/PSRAM heap and DMA largest blocks before treating the new policy as hardware-validated.

后续全平台迁移见[PSRAM_MEMORY.zh-CN.md](PSRAM_MEMORY.zh-CN.md)。2026-10-29，CoreS3
中英文视频固件及开启资源监控的离线音频测试固件通过完整 ESP-IDF v5.5.2 构建和 size 检查。
另有上述 11 个 profile 的 190 个应用/平台源文件通过独立配置编译，未进行这些板型的完整
链接或合并。音视频、输入和显示的 Host 故障注入及单独 UBSan 检查通过，格式、SPDX、
空白检查通过；本机 ASan 不可用。全平台迁移尚无新的真机结果，之前 UI 栈单项迁移的
摄像头启动日志不能代替当前策略的音视频并行、配网、NVS、反复启停和内存回归验证。

## Global boundaries / 全局边界

- Local wake words are not implemented.
- ML307/4G networking is not implemented; all profiles use Wi-Fi.
- Only `m5stack-core-s3` currently implements encoded video uplink.
- NVS encryption, Flash encryption and Secure Boot belong to product provisioning rather than
  these default firmware profiles.

状态变化时只更新本矩阵；专题文档和板卡 README 应链接到这里，不再复制验证状态。
