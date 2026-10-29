# agora-esp32-devkit

[![CI](https://github.com/AgoraIO-Community/agora-esp32-devkit/actions/workflows/ci.yml/badge.svg)](https://github.com/AgoraIO-Community/agora-esp32-devkit/actions/workflows/ci.yml)
[![License](https://img.shields.io/github/license/AgoraIO-Community/agora-esp32-devkit)](LICENSE)

**[English](README.md) | [简体中文](README.zh-CN.md)**

`agora-esp32-devkit` is an ESP-IDF firmware project for running the
[agent-studio-device-firmware](https://github.com/AgoraIO-Community/agent-studio-device-firmware)
voice-chat runtime on
ESP32-S3 boards. The project owns board initialization, Wi-Fi provisioning, persistent storage,
HTTPS transport, audio, input, display, and the firmware lifecycle around the SDK.

The development baseline is **ESP-IDF v5.5.2**. Seventy-two compile-time board profiles are
supported (64 display and 8 headless); the board, Flash, PSRAM, partition table, and hardware
drivers are selected with `MYBOT_BOARD`. See the
[board documentation index](components/mybot_platform/boards/README.md) for hardware details,
controls, build commands, and per-board limitations.

> Project-maintained code is Apache-2.0 unless a file says otherwise. Bundled dependencies and
> media assets have separate terms. Read [License and dependencies](#license-and-dependencies)
> before redistributing source or firmware.

## Features

- Wi-Fi station reconnect and first-boot captive-portal provisioning.
- NVS persistence for device credentials and speaker volume.
- HTTPS with `esp-tls`, the system CA bundle, SNI, and hostname verification.
- 16 kHz mono signed-16 PCM capture and playback with 60 ms packets for the bundled RTSA
  package; other packet durations require a matching RTSA build.
- Agora RTSA full-duplex audio, Cloud AEC, AI QoS, RTM channel subscription, and voice-print
  status.
- A shared bounded PCM playback buffer and independent I2S writer on all board profiles.
- Automatic LVGL selection for display boards, including the CoreS3 pixel-glasses view.
- Localized pairing and Wi-Fi provisioning prompts, with the actual hotspot name shown during
  provisioning.

## Supported Boards

Display profiles:

`zhengchen-1.54tft-ml307`, `zhengchen-1.54tft-wifi`,
`xingzhi-cube-1.54tft-wifi`, `bread-compact-wifi-lcd`, `esp-vocat`,
`esp32-s3-touch-amoled-1.75`, `esp32-s3-touch-amoled-1.75c`,
`esp32-s3-touch-amoled-2.16`, `esp32-s3-touch-amoled-1.8-v2`,
`esp32-s3-touch-lcd-1.85`, `esp32-s3-touch-amoled-1.8`,
`esp32-s3-touch-amoled-1.32`, `esp32-s3-touch-amoled-1.43c`,
`esp32-s3-touch-amoled-2.06`, `atoms3r-echo-base`, `atoms3r-echo-pyramid`,
`sp-esp32-s3-1.28-box`, `esp-box`, `esp-box-3`, `esp-box-lite`,
`esp32s3-korvo2-v3`, `lichuang-dev`, `quandong-s3-dev`, `rymcu-bigsmart`,
`df-k10`, `mixgo-nova`, `taiji-pi-s3`, `m5stack-stopwatch`,
`xingzhi-metal-1.54-wifi`, `xingzhi-cube-0.85tft-wifi`, `xingzhi-abs-2.0`,
`freenove-esp32s3-display-2.8-lcd`, `esp32-s3-touch-lcd-1.54`,
`esp32-s3-touch-lcd-1.83`, `esp32-s3-lcd-0.85`,
`esp32-s3-touch-lcd-1.85b`, `esp32-s3-touch-lcd-1.85c`,
`sp-esp32-s3-1.54-muma`, `atk-dnesp32s3-box0`, `nulllab-ai-vox-v3`,
`aipi-lite`, `magiclick-2p4`, `movecall-cuican-esp32s3`,
`movecall-moji-esp32s3`,
`kevin-sp-v4-dev`, `du-chatx`,
`minsi-k08-dual`, `magiclick-2p5`, `atk-dnesp32s3-box3`,
`genjutech-s3-1.54tft`, `zhengchen-cam`, `labplus-ledong-v2`,
`labplus-mpython-v3`, `atk-dnesp32s3`, `yunliao-s3`,
`lilygo-t-cameraplus-s3`, `lilygo-t-cameraplus-s3-v1.2`, `lilygo-t-circle-s3`,
`jiuchuan-s3`, `lilygo-t-display-s3-pro-mvsrlora`, `atk-dnesp32s3-box`,
`m5stack-core-s3`, `m5stack-stick-s3`, and `sensecap-watcher`.

Headless profiles:

`atom-echos3r`, `xingzhi-cube-0.96oled-wifi`, `doit-s3-aibox`,
`bread-compact-wifi`, `hu-087`, `kevin-box-2`,
`respeaker-flex-xvf3800-circular4-xiao`, and `tudouzi`.

All profiles currently use Wi-Fi. The ML307 profile reserves modem UART pins but does not
implement modem or 4G networking. Only `m5stack-core-s3` currently exposes camera video.

## Build

Install ESP-IDF v5.5.2 using the
[official setup guide](https://docs.espressif.com/projects/esp-idf/en/v5.5.2/esp32s3/get-started/index.html),
then activate it in the current shell:

```sh
. /path/to/esp-idf/export.sh
test "$(idf.py --version)" = "ESP-IDF v5.5.2"
```

Build one board with an isolated build directory and sdkconfig:

```sh
idf.py -B build/<board-id> \
  -DMYBOT_BOARD=<board-id> \
  -DSDKCONFIG=build/<board-id>/sdkconfig \
  -DSDKCONFIG_DEFAULTS="sdkconfig.defaults;ci/ptime60.defaults" build
```

To build both languages for one board or all boards:

```sh
python3 scripts/build_all.py --board <board-id> --language both
python3 scripts/build_all.py
```

The script places intermediate builds in `build/` and merged images in `releases/`. It derives
display and video support from the selected board profile, uses the bundled 60 ms RTSA cadence,
and enables CoreS3 video by default. Use `--video off` to compare without video, `--dry-run` to
inspect commands, and `--no-clean` only for an incremental build.

Build and output roots must be dedicated, non-overlapping, and free of `..` or symbolic-link
components. A normal run cleans roots owned by the script; an existing non-empty root without
the matching `.mybot-generated-root.json` marker is rejected. Safe cleanup requires Python 3.11
or newer. Board-specific direct-build and flash commands are in each
[board README](components/mybot_platform/boards/README.md).

For SenseCAP Watcher, back up the factory-data partition and use `idf.py flash` as described in
its [board README](components/mybot_platform/boards/sensecap-watcher/README.md). Do not run
`erase-flash` on that device.

## Provisioning and Controls

When NVS contains no Wi-Fi credentials, the device creates a configuration AP named
`mybot-xxxx` and serves provisioning at `http://192.168.4.1`. The SDK starts only after the station
has a usable IP address. A provisioning request stops the SDK first and restarts it after
reconnection.

Board inputs use a short press or touch to start/stop a conversation and a 3-second hold to
request provisioning; boards with volume controls persist their value in NVS. Exact inputs and
panel behavior are listed in the
[board documentation index](components/mybot_platform/boards/README.md).

Use `idf.py -B <build-dir> menuconfig` under `mybot` to select the product language, audio packet
duration, Cloud AEC, and AI QoS. The device service endpoint is selected with the language:

- Chinese (`zh-CN`): `https://botstation.shengwang.cn/api`
- English (`en-US`): `https://botstation.agora.io/api`

CPU, heap, and playback diagnostics can be enabled with
`CONFIG_MYBOT_DEBUG_RESOURCE_MONITOR=y`; they are disabled by default. See
[buffered audio playback](docs/AUDIO_PLAYBACK.md) for playback counters and interpretation.
Application data and worker stacks follow the [PSRAM memory policy](docs/PSRAM_MEMORY.md);
hardware DMA, kernel objects and the flash-writing main stack retain internal RAM.

## Validation

Build coverage, recorded real-device results and unsupported areas are maintained in one
[validation matrix](docs/VALIDATION.md). CI builds and host tests do not establish panel behavior,
power sequencing, audio quality, Wi-Fi recovery or other real-device results.

## Documentation

- [Board profiles](components/mybot_platform/boards/README.md)
- [Validation matrix](docs/VALIDATION.md)
- [Board porting](docs/BOARD_PORTING.md)
- [PSRAM memory policy](docs/PSRAM_MEMORY.md)
- [Buffered audio playback](docs/AUDIO_PLAYBACK.md)
- [Platform LVGL UI](docs/PLATFORM_UI.md)
- [CoreS3 LVGL UI](docs/CORES3_UI.md)
- [CoreS3 audio playback comparison](docs/CORES3_AUDIO_TEST.md)
- [CoreS3 camera uplink](docs/CORES3_VIDEO.md)
- [Release checklist](docs/RELEASING.md)
- [Contributing](CONTRIBUTING.md)
- [Support](SUPPORT.md)

## License and Dependencies

Project-maintained code is licensed under [Apache-2.0](LICENSE) unless a file carries another
SPDX identifier. Bundled dependencies and media assets have separate licenses or distribution
terms, including AOSL and Agora RTSA. Review [VENDORED_SOURCES.md](VENDORED_SOURCES.md) and
[THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md) before use or redistribution.
