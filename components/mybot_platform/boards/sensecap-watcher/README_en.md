# SenseCAP Watcher

The board ID in this project is `sensecap-watcher`. [简体中文](README.md).

## Current profile

| Item | Implementation |
| --- | --- |
| SoC | ESP32-S3 |
| Flash | 32 MB QIO, with Quad Flash 32-bit addressing enabled |
| PSRAM | Octal, 80 MHz; capacity is detected on the device |
| Partitions | [partitions/v2/32m-sensecap.csv](../../../../partitions/v2/32m-sensecap.csv), retaining factory data and two 4 MB OTA slots |
| Console | ESP-IDF's default UART0; the board does not override the console type |
| Audio | ES7243E capture and ES8311 playback; SDK boundary: 16 kHz mono signed-16 PCM |
| Display | 412 × 412 SPD2010 over QSPI; shared LVGL view selected automatically |
| Input | Rotary encoder on GPIO41/GPIO42; button read through TCA9555 P0.3 |
| Power | TCA9555 controls system, LCD and codec/amplifier rails |
| Camera | No video adapter in this profile; enabling video is unsupported |

Shared I2C0 uses SDA GPIO47 and SCL GPIO48; TCA9555 has the 7-bit address `0x21`.
I2S0 uses MCLK GPIO10, BCLK GPIO11, WS GPIO12, DIN GPIO15 and DOUT GPIO16.
LCD QSPI uses CLK GPIO7, D0 GPIO9, D1 GPIO1, D2 GPIO14, D3 GPIO13 and CS GPIO45;
the backlight is on GPIO8. Partial updates are aligned to four pixels.

A short encoder-button press starts/stops a conversation; a three-second hold requests Wi-Fi
provisioning. The code maps left rotation to volume up and right rotation to volume down;
verify physical rotation on the device. Volume is persisted in NVS. With no saved Wi-Fi
credentials, provisioning starts automatically, displays the hotspot name and plays a prompt.
Manual provisioning stops MyBot before entering setup.

## Build and flash

Use ESP-IDF v5.5.2 from the repository root:

```sh
source /path/to/esp-idf/export.sh
python3 scripts/build_all.py --board sensecap-watcher --language both
```

The script produces `releases/sensecap-watcher-zh-CN.bin` and its English merged image.
See the [board index](../README.md) for cleanup behavior. To preserve factory data, flash this
device through the dedicated build and `idf.py flash` below; do not write a merged image
across the entire Flash.

Before the first flash, back up the factory partition. Replace the serial port with your
device's port and keep the backup safe:

```sh
python -m esptool --chip esp32s3 --port /dev/ttyUSB0 --baud 2000000 \
  --before default_reset --after hard_reset --no-stub \
  read_flash 0x9000 204800 nvsfactory.bin
```

The `nvsfactory` partition starts at `0x9000` and occupies 200 KiB. Do not run `erase-flash`
or substitute another board's partition table. Standard flashing below does not write
`nvsfactory`:

```sh
idf.py -B build/sensecap-watcher-en-US \
  -DMYBOT_BOARD=sensecap-watcher \
  -DSDKCONFIG=build/sensecap-watcher-en-US/sdkconfig \
  -DSDKCONFIG_DEFAULTS="sdkconfig.defaults;ci/ptime60.defaults;ci/en-us.defaults" build
idf.py -B build/sensecap-watcher-en-US -p /dev/ttyUSB0 flash monitor
```

## Validation scope

This profile has not completed real-device acceptance. Verify power sequencing, the codecs'
16 kHz mode, bidirectional audio, the encoder, provisioning and the display. Camera, touch,
LEDs, battery status, shutdown and low-power support are not integrated. See
[board porting](../../../../docs/BOARD_PORTING.md) and
[platform UI](../../../../docs/PLATFORM_UI.md).
