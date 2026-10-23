# Platform LVGL UI

[English](PLATFORM_UI.md) | [简体中文](PLATFORM_UI.zh-CN.md)

All supported display boards use LVGL. `m5stack-core-s3` selects the process-gray pixel-glasses
view; the other display boards keep the shared workflow view. Selection follows `MYBOT_BOARD`
automatically and sets the hidden `CONFIG_MYBOT_LVGL_UI` symbol. There is no manual renderer
switch or sdkconfig override. The SDK LCD contract, audio/camera paths and touch/key ownership
retain their existing board behavior.

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
| `sensecap-watcher` | SPD2010, 412 × 412 | shared panel adapter | existing encoder path |

The AtomEchoS3R and ReSpeaker Flex profiles have no LCD and do not include LVGL. The ML307 and
Wi-Fi variants, along with the audio/video protocols, are independent of the display backend.
The Xingzhi Cube uses SPI3 mode 3 at 40 MHz, zero offsets, and GPIO13 LEDC backlight with
5 kHz, 13-bit PWM at 60% duty.
Bread Compact Wi-Fi LCD uses SPI3 mode 0 at 40 MHz, zero offsets, RGB order with color inversion,
and GPIO42 LEDC backlight at 5 kHz, 13-bit resolution, and 60% duty. It has one Boot input and
uses the shared view at its portrait geometry.
AMOLED 2.16 uses a zero panel gap and 40 MHz QSPI. Its CO5300 transfer regions retain even pixel
boundaries on both axes.

## CoreS3 pixel-glasses view

CoreS3 alone uses the fixed process-gray skin, 23 expressions and four terminal rows with 9 px
text and a 10 px signature. Six-digit numeric pairing codes remain 32 px. READY shows NEUTRAL;
entering a conversation shows SURPRISED for 300 ms plus its entering transition. LISTENING and
THINKING select their faces, continuous THINKING becomes PROCESSING after three seconds, and
SPEAKING/no phase uses NEUTRAL. The current SDK no longer exposes one-shot emotion indicators;
MCP device commands are limited to `audio.set_volume` and `audio.adjust_volume`. These are
LCD-only changes. See [CoreS3 UI](CORES3_UI.md) for timing, resources and validation.

CoreS3's legacy light-theme setting is retained but does not change the pixel skin. Its animation
option controls expression motion and transitions; disabling it retains the opener hold and
necessary guidance scrolling. The other twelve profiles retain their existing UI/hardware;
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

On screens narrower than 240 pixels (currently StickS3), Chinese labels retain the 20 px bilingual
font; English status, notification, and ordinary labels use 10 px text. The provisioning SSID
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

Current build and device status is tracked only in the [validation matrix](VALIDATION.md).
Host adapter tests cover lifecycle, DMA, copied content and teardown behavior. They do not cover
the complete LVGL page rendering, panel behavior or hardware integration; those remain part of
full LVGL page and real-device regression.
