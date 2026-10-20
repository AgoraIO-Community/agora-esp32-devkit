# SenseCAP Watcher

[中文](README.md)

This guide describes the `sensecap-watcher` profile in `agora-esp32-devkit`. Its configuration is
provided by [board.cmake](board.cmake), [board_config.h](board_config.h), and
[sdkconfig.defaults](sdkconfig.defaults).

## Current configuration

| Item | Configuration |
| --- | --- |
| MCU / storage | ESP32-S3; 32 MB QIO Flash, Octal PSRAM at 80 MHz |
| Audio | ES8311 and ES7243E, 16 kHz mono PCM |
| Audio MCLK/BCLK/WS/DIN/DOUT | GPIO10/11/12/15/16 |
| Display | 412×412 SPD2010 QSPI with the LVGL status view |
| Shared I2C | SDA GPIO47, SCL GPIO48; TCA9555 address `0x21` |
| Input | Rotary encoder GPIO41/42; encoder button on TCA9555 P0.3 |
| Partitions | [`partitions/v2/32m-sensecap.csv`](../../../../partitions/v2/32m-sensecap.csv) |

TCA9555 controls the system, display, and codec-amplifier power sequence. Rotate the encoder to
change volume, short-press it to start or stop a conversation, and hold it for three seconds to
enter provisioning. Camera, touch, LED, battery reporting, shutdown, and low-power operation are
not implemented by this firmware.

## Build

Load ESP-IDF v5.5.2 and run this command from the project root:

```sh
python3 scripts/build_all.py --board sensecap-watcher --language both
```

The builder selects this profile through `MYBOT_BOARD` and creates separate
`build/build-sensecap-watcher-zh-CN/` and `build/build-sensecap-watcher-en-US/` directories.
The languages use their respective service regions. The builder also generates merged images
under `releases/sensecap-watcher-*.bin`; use the segmented flashing procedure below for Watcher
and do not flash those merged images.

## Flash and preserve factory data

The dedicated partition table preserves the `nvsfactory` partition at `0x9000` for 204800 bytes.
Before first flashing, replace the serial port and back up that region. Keep the backup outside
the device in a safe location:

```sh
python -m esptool --chip esp32s3 --port /dev/ttyUSB0 --baud 460800 \
  --before default_reset --after hard_reset --no-stub read_flash 0x9000 204800 nvsfactory.bin
```

Flash the generated English build as separate segments and monitor the device:

```sh
idf.py -B build/build-sensecap-watcher-en-US -p /dev/ttyUSB0 flash monitor
```

Use `build/build-sensecap-watcher-zh-CN` for Chinese. `idf.py flash` uses the selected build's
`flash_args` to write the bootloader, partition table, OTA data, and application segments. The
application starts at `0x200000`; do not write it at `0x0`. These segments exclude factory data.
Do not erase the whole chip or flash a merged image that fills the factory-data gap.

See the [project build guide](../../../../README.md#build) for environment setup and build
options, and the [porting guide](../../../../docs/BOARD_PORTING.md) for validation status.
A successful build does not establish real-device behavior. The reference material's MIT license
is retained in [LICENSE.xiaozhi-esp32](../../assets/LICENSE.xiaozhi-esp32); see the
[third-party notices](../../../../THIRD_PARTY_NOTICES.md) for attribution.
