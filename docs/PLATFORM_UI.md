# Platform LVGL UI

[English](PLATFORM_UI.md) | [简体中文](PLATFORM_UI.zh-CN.md)

All supported display boards use LVGL. `m5stack-core-s3` selects the process-gray pixel-glasses
view; the other display boards keep the shared workflow view. Selection follows `MYBOT_BOARD`
automatically and sets the hidden `CONFIG_MYBOT_LVGL_UI` symbol. There is no manual renderer
switch or sdkconfig override. The agent-studio-device-firmware SDK LCD contract, audio/camera
paths and touch/key ownership retain their existing board behavior.

## Current adapters

| Board | Panel and logical size | LVGL adapter | Input behavior |
| --- | --- | --- | --- |
| `zhengchen-1.54tft-ml307` | ST7789, 240 × 240 | shared ST7789 adapter | existing buttons |
| `zhengchen-1.54tft-wifi` | ST7789, 240 × 240 | shared ST7789 adapter | existing buttons |
| `xingzhi-cube-1.54tft-wifi` | ST7789, 240 × 240 | shared panel adapter | Boot and volume buttons |
| `bread-compact-wifi-lcd` | ST7789, 240 × 320 | shared panel adapter | Boot only |
| `m5stack-core-s3` | ILI9342, 320 × 240 | CoreS3 adapter, pixel-glasses view | existing FT6336 gestures |
| `m5stack-stick-s3` | ST7789P3, 135 × 240 | shared ST7789 adapter | existing main button |
| `esp-vocat` | ST77916, 360 × 360 | shared panel adapter | existing CST816S/Boot path |
| `esp32-s3-touch-amoled-1.75` | CO5300, 466 × 466 | shared panel adapter | existing touch/key path |
| `esp32-s3-touch-amoled-1.75c` | CO5300, 466 × 466 | shared panel adapter | existing touch/key path |
| `esp32-s3-touch-amoled-2.16` | CO5300 480 variant, 480 × 480 | shared panel adapter | CST9217 and Boot |
| `esp32-s3-touch-amoled-1.8-v2` | CO5300 368×448 variant, 368 × 448 | shared panel adapter | CST816S and Boot |
| `esp32-s3-touch-lcd-1.85` | ST77916, 360 × 360 | shared panel adapter | CST816S and Boot |
| `genjutech-s3-1.54tft` | ST7789, 240 × 240 | shared ST7789 adapter | Boot and volume buttons |
| `zhengchen-cam` | ST7789, 320 × 240 | shared ST7789 adapter | Boot and volume buttons |
| `labplus-ledong-v2`, `labplus-mpython-v3` | ST7789-compatible, 320 × 172 | shared ST7789 adapter | Boot |
| `atk-dnesp32s3` | ST7789, 320 × 240 | shared ST7789 adapter | Boot |
| `yunliao-s3` | ST7789, 320 × 240 | shared ST7789 adapter | Boot |
| `lilygo-t-cameraplus-s3`, `lilygo-t-cameraplus-s3-v1.2` | ST7789, 240 × 240 | shared ST7789 adapter | CST816D and Boot |
| `lilygo-t-circle-s3` | GC9D01N, 160 × 160 round | shared panel adapter, round layout | CST816D and Boot |
| `jiuchuan-s3` | GC9309NA, 240 × 296 | shared panel adapter | Boot and volume buttons |
| `sensecap-watcher` | SPD2010, 412 × 412 | shared panel adapter | existing encoder path |
| `xingzhi-metal-1.54-wifi`, `xingzhi-abs-2.0` | ST7789, 240 × 240 | shared panel adapter | Boot/touch or Boot/volume |
| `xingzhi-cube-0.85tft-wifi` | NV3023, 128 × 128 | shared panel adapter | Boot |
| `xingzhi-cube-0.96oled-wifi` | Headless | none | Boot and volume buttons |
| `m5stack-stopwatch` | CO5300, 466 × 466 round | shared panel adapter | CST820B-compatible touch and buttons |
| `esp32-s3-touch-amoled-1.8` | SH8601, 368 × 448 | shared panel adapter | FT5x06 and Boot |
| `esp32-s3-touch-amoled-1.32`, `esp32-s3-touch-amoled-1.43c`, `esp32-s3-touch-amoled-2.06` | SH8601, 466 × 466 or 410 × 502 | shared panel adapter | Boot; FT5x06 on 2.06 |
| `atoms3r-echo-base`, `atoms3r-echo-pyramid` | GC9A01-class, 128 × 128 | shared panel adapter | Boot |
| `sp-esp32-s3-1.28-box` | GC9A01, 240 × 240 round | shared panel adapter | CST816S-compatible touch and Boot |
| `esp-box`, `esp-box-3`, `esp-box-lite` | ILI9341, 320 × 240 | shared panel adapter | Boot; touch not implemented |
| `esp32s3-korvo2-v3`, `df-k10` | ILI9341, 320 × 240 | shared panel adapter | Boot |
| `lichuang-dev`, `rymcu-bigsmart` | ST7789, 320 × 240 | shared panel adapter | FT5x06 or GT911 plus Boot |
| `quandong-s3-dev` | ILI9341, 320 × 240 | shared panel adapter | Boot |
| `mixgo-nova` | ST7789, 128 × 160 | shared panel adapter | Boot |
| `taiji-pi-s3` | ST77916, 360 × 360 round | shared panel adapter | CST816S and Boot |
| `freenove-esp32s3-display-2.8-lcd` | ILI9341, 320 × 240 | shared panel adapter | Boot |
| `esp32-s3-touch-lcd-1.54`, `esp32-s3-touch-lcd-1.83` | ST7789, 240 × 240 or 240 × 284 | shared ST7789 adapter | CST816S and buttons |
| `esp32-s3-lcd-0.85`, `aipi-lite` | ST7789, 128 × 128 | shared ST7789 compact layout | buttons |
| `esp32-s3-touch-lcd-1.85b`, `esp32-s3-touch-lcd-1.85c` | ST77916, 360 × 360 | shared panel adapter | CST816S-compatible touch and Boot |
| `sp-esp32-s3-1.54-muma` | ST7789, 240 × 240 | shared ST7789 adapter | CST816S and Boot |
| `atk-dnesp32s3-box0`, `nulllab-ai-vox-v3` | ST7789, 240 × 240 | shared ST7789 adapter | buttons |
| `magiclick-2p4` | NV3023, 128 × 128 | shared panel adapter | three buttons |
| `movecall-cuican-esp32s3`, `movecall-moji-esp32s3` | GC9A01, 240 × 240 | shared panel adapter | Boot |
| `kevin-sp-v4-dev` | ST7789, 240 × 280 | shared ST7789 adapter | Boot |
| `du-chatx` | ST7789, 128 × 160 | shared ST7789 compact layout | Boot |
| `minsi-k08-dual` | ST7789, 240 × 240 | shared ST7789 adapter | Boot and volume buttons |
| `magiclick-2p5` | GC9107 or ST7735, 128 × 128 | shared compact panel adapter | three buttons |
| `atk-dnesp32s3-box3` | ST7789, 320 × 240 | shared ST7789 adapter | Boot |

The AtomEchoS3R, DOIT S3 AI Box, Bread Compact Wi-Fi, HU-087 and ReSpeaker Flex profiles have no
LCD and do not include LVGL. The ML307 and
Wi-Fi variants, along with the audio/video protocols, are independent of the display backend.
The Xingzhi Cube uses SPI3 mode 3 at 40 MHz, zero offsets, and GPIO13 LEDC backlight with
5 kHz, 13-bit PWM at 60% duty.
Its display and power behavior require real-device validation.
Bread Compact Wi-Fi LCD uses SPI3 mode 0 at 40 MHz, zero offsets, RGB order with color inversion,
and GPIO42 LEDC backlight at 5 kHz, 13-bit resolution, and 60% duty. It has one Boot input and
uses the shared view at its portrait geometry. Chinese and English build support has been validated;
real-device validation remains pending.
AMOLED 2.16 uses a zero panel gap and 40 MHz QSPI. Its CO5300 transfer regions retain even pixel
boundaries on both axes, and touch/panel orientation must be verified on the physical board.
Chinese and English 2.16 build support has been validated; real-device testing remains pending.
AMOLED 1.8 V2 uses a 16-pixel X gap and 40 MHz QSPI with the same even-pixel CO5300 transfer
constraint. LCD 1.85 uses GPIO5 LEDC backlight at 60% and a 360 × 360 ST77916 panel. GenjuTech
1.54 uses ST7789 SPI3 mode 3 at 80 MHz, color inversion, and GPIO2 LEDC backlight at 60%.
Zhengchen Cam uses ST7789 SPI3 mode 0 at 80 MHz with a fixed 320 × 240 landscape layout and an
active-low GPIO42 backlight. Both LabPlus profiles use a 320 × 172 big-endian panel on SPI2 at
20 MHz; Mpython V3 drives CS on GPIO34 and the backlight on GPIO33, while Ledong V2 uses no CS
GPIO, backlight GPIO34, and an STM8-held panel reset.
ATK DNESP32S3 uses ST7789 SPI2 mode 0 at 20 MHz with big-endian pixels and TCA9555-controlled LCD
power. Yunliao S3 uses ST7789 SPI2 mode 0 at 40 MHz with color inversion. T-CameraPlus S3 uses
240 × 240 ST7789 at 60 MHz; its two profiles differ in touch, microphone and speaker-enable pins.
T-Circle S3 uses a 160 × 160 round GC9D01N panel at 40 MHz. Jiuchuan S3 uses SPI3 mode 3 at
80 MHz for its 240 × 296 GC9309NA panel with BGR order.
KEVIN SP V4 Dev uses ST7789 SPI3 mode 3 at 80 MHz, a 20-pixel Y offset, color inversion, and a
GPIO48 LEDC backlight. DuChat X uses ST7789 SPI3 mode 0 at 40 MHz, a 2×1-pixel offset, mirrored
axes, and a GPIO9 LEDC backlight.
Minsi K08 uses ST7789 SPI3 mode 3 at 40 MHz, an 80-pixel X offset, swapped axes and a GPIO7 LEDC
backlight. Magiclick 2.5 selects GC9107 or ST7735 from GPIO3 ADC and uses a GPIO13 LEDC
backlight. ATK Box 3 uses ST7789 on SPI2 at 60 MHz with big-endian pixel data and AW9523B
bit 8 as a binary backlight enable.

## CoreS3 pixel-glasses view

CoreS3 alone uses the fixed process-gray skin, 23 expressions and four terminal rows with 9 px
text and a 10 px signature. Six-digit numeric pairing codes remain 32 px. READY shows NEUTRAL;
entering a conversation shows SURPRISED for 300 ms plus its entering transition. LISTENING and
THINKING select their faces, continuous THINKING becomes PROCESSING after three seconds, and
SPEAKING/no phase uses NEUTRAL. The current SDK does not expose one-shot emotion indicators; its
MCP path accepts only `audio.set_volume` and `audio.adjust_volume` inside a `message.user`
envelope.
These are LCD-only changes. See [CoreS3 UI](CORES3_UI.md) for timing, resources and validation.

CoreS3's legacy light-theme setting is retained but does not change the pixel skin. Its animation
option controls expression motion and transitions; disabling it retains the opener hold and
necessary guidance scrolling. The other profiles retain their existing UI/hardware;
SDK, AOSL, RTSA, camera/video policy, input, power and memory configuration are unchanged.

## Shared workflow view on other display boards

The view receives semantic SDK LCD content: the ten workflow screens, pairing code, voiceprint
indicator, and listening/thinking/speaking state. Coordinates are derived from each panel's
logical width and height; the view does not assume the CoreS3 320 × 240 geometry. Small local
state images are predecoded into Flash; no runtime PNG/GIF decoder or full-screen cache is added.

The provisioning screen shows the actual device SoftAP SSID (`mybot-xxxx`) in the center and a
Chinese/English connection hint below. Its title and hint scroll in a loop only when wider than
their labels; short text stays still, and scrolling stops on exit. This text scrolling remains
available when `CONFIG_MYBOT_LVGL_UI_ANIMATIONS` disables state activity animation.

The pairing-code screen directs users to enter the code in the web console. Its instruction
scrolls only when wider than the footer label and stops scrolling on exit.

On screens narrower than 240 pixels, Chinese labels retain the 20 px bilingual font; English
status, notification, and ordinary labels use 10 px text. The provisioning SSID
uses 20 px in both languages and scrolls if needed. Pairing codes select a 32, 20, or 10 px font
to fit, with tighter spacing for long codes on narrow screens. The built-in Chinese subset covers
the UI labels, not arbitrary conversation text.

Touch and button events remain in each board's input driver. The UI does not add a new
gesture, alter provisioning, or change the audio/video lifecycle. The backend uses one partial
RGB565 DMA stripe of 16 rows (`width × 16 × 2` bytes) and a dedicated LVGL task on Core 1 at
priority 1. Objects and drawing scratch space use PSRAM; the DMA stripe uses internal memory.
CoreS3's stripe is 10,240 bytes, StickS3's is 4,320 bytes, and AMOLED 2.16's is 15,360 bytes,
separate from task stacks and driver
allocations. The hardware integration owns reset, offsets, color order, backlight, and teardown.

Round profiles set `MYBOT_DISPLAY_ROUND=1`. The shared view then narrows the header, card and
footer to a central safe rectangle instead of placing widgets in the panel corners. This layout
logic is build-verified only; color, orientation, clipping and touch alignment still require
real-device checks for each panel.

## Build examples

Use a separate build directory for every board and language. No extra UI defaults are required:

```sh
idf.py -B build/esp-vocat-lvgl \
  -DMYBOT_BOARD=esp-vocat \
  -DSDKCONFIG=build/esp-vocat-lvgl/sdkconfig \
  -DSDKCONFIG_DEFAULTS=sdkconfig.defaults build
```

Theme and activity-animation settings remain build options; no runtime theme gesture is
introduced. On the shared workflow view, append `ci/lvgl-ui-light.defaults` for the light theme
or `ci/lvgl-ui-static.defaults` for static activity indicators. CoreS3 keeps process gray even
with the light-theme defaults; static defaults disable its expression animations. Use a new
sdkconfig when changing defaults, because defaults do not override saved configuration.

## Validation status

The earlier CoreS3 status-card UI passed real-device testing. The new CoreS3 pixel-glasses view
passed its initial Chinese/English ESP-IDF v5.5.2 standard-builder video builds and still needs
host/real-device validation; previous results do not
validate its layout, expression timing or resource load. The shared-view cleanup, provisioning
SSID display/scrolling and narrow-screen font changes retain their existing regression needs.
CI defines all thirteen profiles in both languages, CoreS3 video in both languages and two
theme/animation variants (30 configurations); that matrix is not a claim that this change has
completed all builds. No new board is claimed to have passed real-device color, touch, audio,
provisioning, or teardown validation. Complete hardware testing must cover all workflow screens,
orientation and colors, Chinese/English small-screen text, SSID and hint scrolling with activity
animation disabled, touch/buttons, Wi-Fi recovery, repeated SDK stop/start, full-duplex audio,
and memory/CPU measurements.
