# CoreS3 pixel UI adapter regression

Run `sh tests/cores3_pixel_ui/run.sh`. The test compiles the real CoreS3 adapter
and expression policy with the existing ST7789 host declarations and mocked
CoreS3 panel, LVGL port, Wi-Fi SSID snapshot, and view. It does not use hardware.

Coverage includes partial initialization failures, retryable cleanup, board/SDK
display references, pending DMA callbacks, copied pair codes and SSIDs, runtime
status, same-emotion replay, phase transitions, foreground expiry, hangup and
detach cancellation, malformed emotions, and a new command arriving between
view submission and foreground-clock acknowledgement.
