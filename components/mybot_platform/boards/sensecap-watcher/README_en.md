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

## Factory-data protection

To preserve factory data, use the dedicated build and `idf.py flash` instructions in the
[board index](../README.md); do not write a merged image across the entire Flash.

Before the first flash, back up the factory partition. Replace the serial port with your
device's port and keep the backup safe:

```sh
python -m esptool --chip esp32s3 --port /dev/ttyUSB0 --baud 2000000 \
  --before default_reset --after hard_reset --no-stub \
  read_flash 0x9000 204800 nvsfactory.bin
```

The `nvsfactory` partition starts at `0x9000` and occupies 200 KiB. Do not run `erase-flash`
or substitute another board's partition table. Standard `idf.py flash` does not write
`nvsfactory`. Build and validation status: see the [board index](../README.md) and
[validation matrix](../../../../docs/VALIDATION.md).
