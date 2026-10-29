# CoreS3 pixel UI adapter regression

Run `sh tests/cores3_pixel_ui/run.sh` with CMake and a host C/C++ compiler installed.
The adapter test compiles the real CoreS3 adapter
and expression policy with the existing ST7789 host declarations and mocked
CoreS3 panel, LVGL port, Wi-Fi SSID snapshot, and view. It does not use hardware.

Coverage includes the 7-KiB PSRAM stack request, stack allocation and readiness
failures, partial initialization failures, retryable cleanup, board/SDK display
references, pending DMA callbacks and teardown timeout, copied pair codes and SSIDs, runtime
status, automatic phase transitions, opener timing and retry, unknown indicator
mask handling, hangup and detach cancellation.

The renderer test links the complete pinned LVGL runtime with the real CoreS3 view,
fonts and expressions. It injects failure at PSRAM-state allocation and every
renderer-owned widget/timer creation step, then covers repeated create/destroy,
expression timer callbacks, stable geometry/userdata addresses and timer-before-widget
teardown. It checks that view state is freed after widget callbacks, all timers are
removed and all LVGL allocations are released at shutdown.

The port mock tracks stack allocation and release across failure, retry and repeated
initialization. These host tests do not establish actual PSRAM stack safety or DMA availability
on the device. Set `CORES3_PIXEL_UI_SANITIZERS=1` to enable AddressSanitizer and
UndefinedBehaviorSanitizer.
