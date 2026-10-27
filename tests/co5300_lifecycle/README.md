# CO5300 Panel Host Tests

```sh
sh tests/co5300_lifecycle/run.sh
```

This compiles the original production panel against the AMOLED 1.75 and 1.75C
profiles, the separate 480-pixel panel against the 2.16 profile, and the
368x448 panel against the 1.8 V2 profile. Constructor mocks inspect the initialization commands,
address windows, QSPI configuration, panel dimensions, reset pin, and brightness.
Each panel variant's command bytes and delays are checked for compatibility.

Fault injection covers initialization errors, partially returned handles,
cleanup timeouts, retries, and repeated open/close. The existing
`tests/st7789_lifecycle/run.sh` covers shared LVGL callback drain and timeout
behavior. Both runners reuse host ESP-IDF declarations.

A separate executable links the production 2.16 board lifecycle with host LCD,
hardware, and input mocks. It verifies initialization failure cleanup, retained
touch IO across two consecutive input cleanup failures, hardware cleanup retry,
resource release order, and idempotent preparation after success. Each scenario
runs in a fresh process. These tests do not exercise the shared input worker's
join timing.

Host results do not validate physical display colors, panel timing, brightness,
or power sequencing.
