# Vendored Dependency Baselines

This file records pinned build inputs, delivered assets and platform implementation baselines. License and
redistribution terms are documented separately in [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md).

| Path | Source | Pinned revision |
| --- | --- | --- |
| `components/mybot_stack/mybot_sdk/mybot` | [agent-studio-device-firmware](https://github.com/AgoraIO-Community/agent-studio-device-firmware/tree/46e265a960fac48d93ae6755bdf694aeb09ab366) | v1.3.0, `main` commit `46e265a960fac48d93ae6755bdf694aeb09ab366` |
| `components/mybot_stack/aosl` | `github.com/AgoraIO-Community/aosl` | v1.0.4 + post-release fixes, commit `84e086084ebcd0ae2455a0ce5721950c5fe2e656` |
| `components/mybot_stack/agora_rtc` | Agora RTSA Lite package archive | v1.10.1, build 1270872 (`20260828_194128`) |
| `components/esp-wifi-connect` | `github.com/78/esp-wifi-connect` | 3.2.2, commit `c24b97c194e6b4a1d7be0237b3c28980661cac1e` |
| `components/esp_audio_codec` | ESP Component Registry `espressif/esp_audio_codec` | 2.5.0, commit `3bb83597d07b604e1ab5b78dd4370a28d6fa802d` |
| `components/esp_codec_dev` | `github.com/espressif/esp-adf` | 1.5.11, commit `73befa9ebffdd6e5065b7145329f115910e13ab5` |
| `components/esp_video` | `github.com/espressif/esp-video-components`, `esp_video` | 2.3.0, commit `58d4c6eea08f0f78a2beff27aa4e0155efd74b55` |
| `components/esp_cam_sensor` | `github.com/espressif/esp-video-components`, `esp_cam_sensor` | 2.3.0, commit `58d4c6eea08f0f78a2beff27aa4e0155efd74b55` |
| `components/esp_sccb_intf` | `github.com/espressif/esp-video-components`, `esp_sccb_intf` | 0.0.8, commit `3384d3510c5880edeb0f4c14d9e0196760280637` |
| `components/esp_new_jpeg` | `github.com/espressif/esp-adf-libs`, `esp_new_jpeg` | 0.6.1, commit `35c37e1656db36dbf13b9aee8064d1b59f482f6c`; ESP32-S3 library SHA-256 `4205b1258ce0ef9fd9946abfc7cd5f105b08316ea567e1136cec60b3e6330896` |
| `components/esp_lcd_ili9341` | `github.com/espressif/esp-bsp` | 2.0.2, commit `fc8bd325efcdef6d5802554659debba303058af6` |
| `components/lvgl` | `github.com/lvgl/lvgl` | 9.5.0, commit `85aa60d18b3d5e5588d7b247abf90198f07c8a63` |
| `components/esp_lvgl_port` | `github.com/espressif/esp-bsp`, `components/esp_lvgl_port` | 2.8.0~1, commit `d14ff131266bf1392efff88db72cb4638897507c` |
| `components/esp_lcd_spd2010` | ESP Component Registry `espressif/esp_lcd_spd2010` | 2.0.0~1, commit `12f6ca1182ec48889b17ec570fadaaf267cb336e` |
| `components/esp_lcd_co5300` | ESP Component Registry `espressif/esp_lcd_co5300` | 2.1.0, commit `56f3c5620464c061e968d3358bc897c528e097ee`, content hash `21f84c5c825509ebacbddb379555b49092ae165b3e19f0edfbf6f8eff8aa036d` |
| `components/esp_lcd_st77916` | ESP Component Registry `espressif/esp_lcd_st77916` | 2.0.2, commit `91aeb7fb41e8a3e76aeb21371f9f83711c74cf3f` |
| `components/esp_lcd_touch` | ESP Component Registry `espressif/esp_lcd_touch` | 1.2.1, commit `c927778a85eed239dd403c1719d4f543ad56e693`, content hash `3f85a7d95af876f1a6ecca8eb90a81614890d0f03a038390804e5a77e2caf862` |
| `components/esp_lcd_touch_cst816s` | ESP Component Registry `espressif/esp_lcd_touch_cst816s` | 1.1.1~1, commit `ab5696c61e761b272a163740f94b0e5720c3dde4` |
| `components/esp_lcd_touch_cst9217` | ESP Component Registry `waveshare/esp_lcd_touch_cst9217` | 1.0.4, commit `43e6e696bbf0cf8671a97fcfe02c4c720e4e4d27`, content hash `27a00845832b7987cacf4c4125cbed5415d940987583065b01870ac080930856` |
| `components/esp_io_expander` | ESP Component Registry `espressif/esp_io_expander` | 1.2.1, commit `eb76dc6ecf21ccc4ee7ee58bfea3d3d31fa090cf` |
| `components/esp_io_expander_tca95xx_16bit` | ESP Component Registry `espressif/esp_io_expander_tca95xx_16bit` | 2.0.2, commit `53f6127ba3a1dd80fbdf9a76b759ccd1a8dc0101` |
| `components/button` | ESP Component Registry `espressif/button` | 4.2.0, commit `5f9cb98ae4d0e8153c4b4d1accf471214e5b6fe8` |
| `components/knob` | ESP Component Registry `espressif/knob` | 1.1.0, commit `5f9cb98ae4d0e8153c4b4d1accf471214e5b6fe8` |
| `components/cmake_utilities` | ESP Component Registry `espressif/cmake_utilities` | 0.5.0 |
| `components/m5pm1` | ESP Component Registry `m5stack/m5pm1` | 1.0.7, content hash `731f79d0629e245787440f5419aac5d7a82befeb25f97689d6a2d0331a24a72d` |
| `components/mybot_platform/src/drivers/display/renderers/lvgl/lvgl_fonts.c`, `components/mybot_platform/src/drivers/display/assets/licenses/LVGL_VIEW_FONT_LICENSE.txt` | `github.com/lvgl/lvgl`, `scripts/built_in_font/SourceHanSansSC-Normal.otf` | LVGL 9.5.0 source font SHA-256 `1ee89e1669362dee13851129c0a8a791a87521eb4148e5efbf5d26596738e25b`; generated 20 px, 4 bpp static UI subset |
| `components/mybot_platform/assets/ui/noto_emoji`, `components/mybot_platform/src/drivers/display/renderers/lvgl/lvgl_assets.c` | Noto Color Emoji font glyphs rasterized by `github.com/78/noto-fonts`, `png/noto-color-emoji_64` | 2.0.0, commit `d45dbc64052d57048f20ab1770074172ce9eb53b`; per-image SHA-256 values and glyph provenance in `components/mybot_platform/assets/ui/noto_emoji/SOURCES.json` |

The SDK's public headers and core sources match the pinned official commit. ESP-IDF integration
also supplies the generated `include/mybot/mybot_version.h`. The official Git tree IDs are
`f0f111a3b7790a650dac9cb29c93edc2c52a5086` for `include/` and
`a6b52ae939875a49f0d8b04b512358aa9edea849` for `src/`. In a checkout of the SDK repository,
`git archive 46e265a960fac48d93ae6755bdf694aeb09ab366 include src | sha256sum` produces
`6c6717a50fab6132443a9fe98216b32d9edfc95b11a5a18514f72394c59f873d`.

## Board reference documents

Per-board hardware notes and usage instructions are indexed in
[the board documentation index](components/mybot_platform/boards/README.md).
These pages describe the checked-in `board.cmake`, `board_config.h` and board defaults,
using this repository's `MYBOT_BOARD` selector and `scripts/build_all.py` build entry.
Firmware capabilities and validation limits follow the current profiles and the English
and Chinese root READMEs.

| Board profile | Delivered description |
| --- | --- |
| `atom-echos3r` | [README](components/mybot_platform/boards/atom-echos3r/README.md) |
| `bread-compact-wifi-lcd` | [README](components/mybot_platform/boards/bread-compact-wifi-lcd/README.md) |
| `esp-vocat` | [README](components/mybot_platform/boards/esp-vocat/README.md) |
| `esp32-s3-touch-amoled-1.75` | [README](components/mybot_platform/boards/esp32-s3-touch-amoled-1.75/README.md) |
| `esp32-s3-touch-amoled-1.75c` | [README](components/mybot_platform/boards/esp32-s3-touch-amoled-1.75c/README.md) |
| `esp32-s3-touch-amoled-2.16` | [README](components/mybot_platform/boards/esp32-s3-touch-amoled-2.16/README.md) |
| `m5stack-core-s3` | [README](components/mybot_platform/boards/m5stack-core-s3/README.md) |
| `m5stack-stick-s3` | [README](components/mybot_platform/boards/m5stack-stick-s3/README.md) |
| `respeaker-flex-xvf3800-circular4-xiao` | [README](components/mybot_platform/boards/respeaker-flex-xvf3800-circular4-xiao/README.md) |
| `sensecap-watcher` | [README](components/mybot_platform/boards/sensecap-watcher/README.md) / [English](components/mybot_platform/boards/sensecap-watcher/README_en.md) |
| `xingzhi-cube-1.54tft-wifi` | [README](components/mybot_platform/boards/xingzhi-cube-1.54tft-wifi/README.md) |
| `zhengchen-1.54tft-ml307` | [README](components/mybot_platform/boards/zhengchen-1.54tft-ml307/README.md) |
| `zhengchen-1.54tft-wifi` | [README](components/mybot_platform/boards/zhengchen-1.54tft-wifi/README.md) |

## Platform implementation references

The platform table identifies third-party derivation and hardware references separately from
the delivered adaptations. [ESP32 implementation baseline][esp32-platform-baseline] refers to
the listed paths at the published `agora-esp32-devkit` commit
`1bc740a8f4c5e3ba188071d50a1623d9fe328294`. It provides the adapted platform code and its
retained file-level notices. The original projects' application layers and dependency sets
are not included by these references.

| Paths | Derivation or hardware reference | Delivered implementation or pinned input |
| --- | --- | --- |
| CoreS3 `components/mybot_platform/src/drivers/display/renderers/lvgl/pixel_glasses_view.cc`, expression geometry/masks/generators, `pixel_expressions.c`, and private expression headers/policy | BK7259 pixel-glasses renderer family, retaining MIT/Apache-2.0 file notices | [ESP32 implementation baseline][esp32-platform-baseline]; adapted to the ESP32 display API and CoreS3 320x240 geometry |
| CoreS3 terminal subsets in `components/mybot_platform/src/drivers/display/renderers/lvgl/pixel_fonts.c` and `TERMINAL_FONT_LICENSE.txt` | DejaVu Sans Mono Bold, renamed MyBot Terminal | Source TTF SHA-256 `3a3c502eeff669a231549e80df9f7c49de109bafe303170409e905d0b31a38fe`; 9 px terminal and 10 px signature subsets, with retained Bitstream Vera terms |
| `components/mybot_platform/src/drivers/display/adapters/lvgl/shared_lvgl_adapter.cc`, `components/mybot_platform/src/internal/display/display_panel.h` | Project-maintained shared adapter for ST7789, CO5300, SPD2010, and ST77916 panels | Uses the board panel drivers; no new upstream source |
| `components/mybot_platform/boards/respeaker-flex-xvf3800-circular4-xiao`, `components/mybot_platform/src/drivers/audio/xvf3800_audio.c`, `partitions/v2/8m.csv` | `github.com/qiuyanli1990/respeaker-flex-circle-Agora-mybot` | commit `b06024382eb104c998aead4841e1df647193065b` |
| `components/mybot_platform/boards/bread-compact-wifi-lcd/bread_input.c` | Existing local `components/mybot_platform/boards/atom-echos3r/atom_echos3r_input.c` MIT template | Project-local adaptation for Boot GPIO0; retains MIT copyright and permission notice |
| `components/mybot_platform/boards/esp-vocat/board_config.h` (hardware mapping verification only) | `github.com/espressif/esp-brookesia` | commit `b22c488f50bafe53342c8e171081bd736396ef58` |

[esp32-platform-baseline]: https://github.com/AgoraIO-Community/agora-esp32-devkit/tree/1bc740a8f4c5e3ba188071d50a1623d9fe328294

Firmware integration differences are limited to the active ESP32-S3 build:

- Only `m5stack-core-s3` selects the process-gray pixel-glasses view. CMake chooses it from
  `MYBOT_BOARD`, with no manual renderer flag; the other twelve profiles retain their current
  UI/hardware. The original Source Han Sans 20 px UI subset is reused, while the CoreS3 path
  uses fixed widgets/curves, five immutable A8 masks (13,631 alpha bytes), and the terminal
  subsets above instead of the shared view's four emoji. Audio, camera/video policy, input,
  power, PSRAM, partitions, SDK, AOSL and RTSA inputs are unchanged.
  Geometry, colors, masks and generators are recorded in the delivered
  [pixel-glasses resource guide](components/mybot_platform/src/drivers/display/renderers/lvgl/PIXEL_GLASSES_SOURCES.md).
  MIT/Apache-2.0 notices apply to the reused BK display sources/generators as stated in their files.
- The Xingzhi Cube port reuses project audio/input/LVGL drivers and introduces no new vendored
  dependency. Its Board lifecycle is project-maintained; the reference supplies hardware facts.
  The 16 MB Flash and 80 MHz Octal PSRAM profile requires real-device capacity confirmation.
- The Bread Compact Wi-Fi LCD port fixes the user-selected N16R8, 240x320 ST7789, and separate
  I2S wiring. It reuses raw audio, the existing ST7789 panel, and shared LVGL; its independent
  Boot-only input is adapted from the local MIT AtomEchoS3R input template. Board lifecycle and
  build metadata are project-maintained Apache-2.0. Chinese and English build support has been
  validated; real-device validation remains pending.
- The AMOLED 2.16 profile owns independent MIT Board and CO5300 480-pixel panel variants derived
  from the existing integration. It reuses AXP2101 configuration, codecs, CST9217, and shared LVGL
  with independent identity and 480x480 zero-gap geometry.
  Its conservative 16 MB Flash/Octal PSRAM defaults require physical capacity confirmation;
  no new dependency or upstream application service is added.
- LVGL and its ESP-IDF port are local, pinned dependencies included automatically for display
  boards and omitted for headless boards. The build omits examples, tests, and registry download metadata. The UI uses
  a static bilingual font subset and existing public LCD state; no upstream application services,
  asset download protocol, or SDK internals are imported.
  `scripts/generate-lvgl-font.py` generates the font using Pillow 10.2.0 and FreeType 2.13.2;
  the checked-in glyph data is sufficient for firmware builds without the generator or source OTF.
  LVGL's local allocator keeps its heap in PSRAM; local component configuration selects the
  required rendering features and fonts without changing its upstream source files. Integration
  details are in `components/lvgl/README.integration.md`. The ESP-IDF port's runtime, display
  rollback, and deinit contract are adapted for repeated initialization and teardown; the exact
  local corrections are recorded in `components/esp_lvgl_port/PATCHES.md`.
  The shared workflow view on other display boards enables the unchanged upstream LVGL image widget. Four pinned local emoji PNGs
  are converted by `scripts/generate-lvgl-ui-assets.py` with Pillow 10.2.0 into 64x64 straight-alpha
  ARGB8888 constants (BGRA byte order), totaling 65,536 pixel bytes in Flash. The script pads the
  64x61 source images without scaling; the firmware does not include PNG/GIF decoding or an image
  cache. Original PNGs and their font license are retained for reproducibility.

- The video components include only the GC0308 DVP path, native I2C SCCB, and the ESP32-S3 JPEG
  library. Local CMake/Kconfig replace registry metadata and pin component versions; sensor
  detection uses the static table, the default format is QVGA YUYV, and SCCB timeout is 100 ms.
  The local DVP DMA-size default is 16,384 bytes with video enabled; the driver's frame alignment
  allocates 15 KiB of internal DMA memory for QVGA YUYV. The platform supplies capture buffers
  at most five times per second while retaining the sensor's 20 fps register configuration.
  Unused transports, sensors, examples, and registry caches are omitted. Video sources and the
  JPEG binary are linked only with `CONFIG_MYBOT_ENABLE_VIDEO=y`.

- The AOSL ESP32-S3 HAL uses FreeRTOS delays, byte-sized task stacks, PSRAM allocation, and IPv4
  DSCP/TOS support required by the bundled RTSA package.
- Component Registry manifests and download-cache checksum metadata are omitted because dependencies
  are local or supplied by ESP-IDF v5.5.2.
- The codec-device source set contains only the common adapters and ES8311, ES7243E, ES7210, and
  AW88298 devices used by supported boards; codec-setting errors are propagated to platform callers.
- The TCA95xx constructor removes its I2C device if register reset fails.
- The SenseCAP profile uses explicit IO-expander calls and disables the optional global GPIO API
  wrapper.
- The Wi-Fi component accepts an explicit provisioning SSID and waits for its DNS worker at teardown.
- The button, knob, and SPD2010 components define their pinned version macros without Component
  Registry manifests.
- The M5PM1 production sources retain upstream content with line endings normalized to LF and use
  ESP-IDF's native I2C master API; the optional `i2c_bus` dependency, examples, datasheets, and
  registry metadata are not vendored.
- The CO5300 and ST77916 components contain only the production paths required by supported boards;
  unused interfaces, tests, registry metadata, and caches are omitted. The LCD-touch components
  likewise omit tests and registry metadata.
- The ST77916 and CST816S production sources have formatting normalized for this repository. The
  CST816S unused ID helper follows its Kconfig guard so the ESP-VoCat no-ID build remains warning
  free.

When updating a dependency, update this file and `THIRD_PARTY_NOTICES.md`, build from a clean
sdkconfig, and report the board validation performed.
