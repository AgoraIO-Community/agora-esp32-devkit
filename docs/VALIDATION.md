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

## Global boundaries / 全局边界

- Local wake words are not implemented.
- ML307/4G networking is not implemented; all profiles use Wi-Fi.
- Only `m5stack-core-s3` currently implements encoded video uplink.
- NVS encryption, Flash encryption and Secure Boot belong to product provisioning rather than
  these default firmware profiles.

状态变化时只更新本矩阵；专题文档和板卡 README 应链接到这里，不再复制验证状态。
