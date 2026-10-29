# CoreS3 video uplink

[English](CORES3_VIDEO.md) | [简体中文](CORES3_VIDEO.zh-CN.md)

The optional `m5stack-core-s3` camera adapter captures GC0308 QVGA YUYV frames and encodes complete
JPEG images for the mybot public video callback. Uplink is capped at one frame per second, including
when RTC rejects a frame. The sensor retains its 20 fps timing, while the platform requests actual
DVP capture at most five times per second. Only one buffer is queued at a time, with at least
200 ms between requests. After a frame completes, the empty queue stops DMA and PSRAM copying
until the next sample; sensor exposure and VSYNC continue. Only selected samples are encoded.
Slow sending does not accumulate old frames or trigger catch-up capture.
The adapter is
[`cores3_camera_video.c`](../components/mybot_platform/src/drivers/video/cores3_camera_video.c).

## Build and enable

Activate ESP-IDF v5.5.2, then run from the repository root with an isolated configuration:

```sh
idf.py -B build/cores3-video \
  -DMYBOT_BOARD=m5stack-core-s3 \
  -DSDKCONFIG=build/cores3-video/sdkconfig \
  -DSDKCONFIG_DEFAULTS="sdkconfig.defaults;ci/video.defaults" reconfigure build
idf.py -B build/cores3-video -p <PORT> flash monitor
```

For an existing CoreS3 build, enable `mybot → Enable CoreS3 camera JPEG uplink (1 fps)` in
menuconfig for that build directory. `CONFIG_MYBOT_ENABLE_VIDEO` defaults to `n`. Other boards
reject the enabled configuration, including `bread-compact-wifi-lcd`, `xingzhi-cube-1.54tft-wifi`, and
`esp32-s3-touch-amoled-2.16`, which have no camera path.
CoreS3-SE has no GC0308 and is not a supported video target.
Keep the audio packet duration at the supported 60 ms. A fresh build uses Chinese; append
`;ci/en-us.defaults` to the defaults list in a new build directory for English. All CoreS3 builds
use LVGL; no separate UI enable flag is needed.
When reusing a build directory, run `idf.py -B build/cores3-video reconfigure build` so changes
to the project's hidden DMA default are regenerated. Confirm
`CONFIG_CAM_CTRL_DVP_DMA_BUFFER_SIZE=8192` in that build's generated `sdkconfig`.
The platform [PSRAM memory policy](PSRAM_MEMORY.md) automatically selects
`CONFIG_FREERTOS_TASK_CREATE_ALLOW_EXT_MEM=y` for application worker and UI stacks.

## Hardware and memory

| Signal | Wiring |
| --- | --- |
| SCCB | Shared native I2C1, SDA12/SCL11, GC0308 address `0x21`, 100 kHz |
| D0–D7 | GPIO39/40/41/42/15/16/48/47 |
| VSYNC/HREF/PCLK | GPIO46/38/45 |
| XCLK | External 20 MHz oscillator, no GPIO output |
| Reset | AW9523 P1_0; locked masked writes preserve LCD and audio reset lines |

Two 153,600-byte capture buffers and one 131,072-byte JPEG buffer use about 428 KiB of PSRAM.
The encoder also allocates workspace. With video enabled,
[`main/Kconfig.projbuild`](../main/Kconfig.projbuild) sets the hidden
`CAM_CTRL_DVP_DMA_BUFFER_SIZE` default to 8,192 bytes before the vendored component's default is
loaded. The vendored Kconfig and driver remain unchanged. For 320×240 YUYV, the driver chooses
two 3,840-byte halves that divide the 153,600-byte frame, allocating 7,680 bytes (7.5 KiB) of
contiguous internal DMA memory, plus descriptors and a 3-KiB capture-task stack. The earlier
16,384-byte setting allocated 15,360 bytes (15 KiB). Check the generated DMA configuration when
comparing firmware. The encoding worker has a 6-KiB PSRAM stack; the vendor capture worker
retains its internal stack. Both PSRAM frame buffers remain allocated and rotate through the
single queued slot.

`no mem for CAM DVP DMA receive buffer` means the driver could not allocate its contiguous
internal DMA buffer when streaming starts, after RTC has connected. Free PSRAM cannot satisfy
this allocation; total free internal memory can also exceed the request while its largest DMA
block is too small. Descriptors, task stacks and other runtime allocations still need additional
memory. LVGL objects use PSRAM and the UI has a separate 10 KiB internal DMA buffer; it does not
allocate a full-screen page cache.

Before moving the UI stack, a device log still reported allocation failure with
`dma_free=13187` and `dma_largest=5120`: the largest block was below the 7,680-byte requirement.
The CoreS3 adapter now places its 7,168-byte LVGL task stack in PSRAM through the existing port's
`task_stack_caps`, moving 7 KiB of stack storage out of internal RAM. The display DMA stripe stays
internal. This change does not statically guarantee a sufficient contiguous block or successful
camera startup; check the new device logs.

The smaller buffer produces about 40 DMA data events per active frame instead of 20 and leaves
less time to service each half before reuse. The sensor's 20 fps timing during capture is unchanged
by the 5 fps request or 1 fps upload limits. Validate capture errors, timeouts and audio continuity
on a device with RTC audio and the UI active; this memory reduction alone does not establish that
the reported hardware failure is resolved.

The JPEG worker uses priority 3 without fixed core affinity and no additional encoder helper task.
The DVP copy task retains the upstream driver's higher priority for timely DMA service.
This path does not use the audio
I2S peripheral. The LCD continues to show workflow/status pages; no camera preview is rendered.

## Lifecycle and bandwidth

Initialization allocates resources but does not start DVP capture or invoke the SDK frame callback.
After RTC connects, `start` schedules the worker. JPEG buffers remain unchanged until the send
callback returns. Capture dequeues poll with a 100 ms timeout; a timeout keeps ownership with the
driver and never queues a duplicate buffer. If no frame arrives within one second, the worker
logs an error and stops the stream. This also bounds the driver's retries of malformed frames;
the 5 fps limit describes normal sampling, not each failed hardware retry.
Stop waits up to 3 seconds for the worker
and in-flight callback; if still pending, it retains resources and reports failure for SDK retry.
After completion the worker suspends; its owner confirms suspension and deletes it with
`vTaskDeleteWithCaps` before releasing resources. Completion remains retryable after a join
timeout. Only a fully stopped source is destroyed. Camera teardown never deletes the Board-owned
I2C bus.

The advertised bitrate range is 64–512 kbit/s, starting at 288 kbit/s. JPEG quality starts at 60;
bandwidth feedback changes quality within 30–60. Frames exceeding the current one-second byte
budget or the fixed output buffer are dropped. Therefore poor bandwidth or complex images may
result in fewer than one delivered frame per second. SDK acceptance does not prove server inference.

## Diagnostics and acceptance

Current build and hardware status is tracked only in the
[validation matrix](VALIDATION.md). The following checks remain the device and server acceptance
procedure.

The display initialization log includes `ui_stack_bytes=7168 ui_stack=psram`.
`event=video_memory` records `stage=before_stream_on` and, if streaming does not start,
`stage=stream_on_not_started`. Its fields are `dma_buffer_max` (the configured upper bound,
8,192 bytes by default), `dma_free`, `dma_largest`, `internal_free`, and `psram_free`, all in bytes.
`dma_free` and `dma_largest` use the driver's internal DMA memory capabilities. Compare the
pre-start `dma_largest` with the actual 7,680-byte buffer requirement; sufficient space for this
buffer does not guarantee enough memory for the rest of stream startup.

`cores3_video` emits lifecycle logs and `event=video_stats` every 10 seconds during streaming:
`window_ms`, `captured`, `encoded`, `sent`, `rejected`, `dropped`, `capture_errors`, `last_bytes`, `quality`, `target_bps`,
and `encode_max_us`. Counters and `encode_max_us` reset each interval; `last_bytes`, `quality`, and
`target_bps` retain their latest values. `captured` counts successful frame dequeues, not every
DMA attempt; divide by `window_ms / 1000` to measure the delivered capture rate. Frames not
requested while the queue is empty do not count as `dropped`. `sent` means RTSA accepted the SDK
callback. Normal capture can approach 50 frames per 10 seconds; encoding/send duration and
200 ms sampling intervals can put uplink near 8–10 frames per 10 seconds, always capped at 1 fps.

Validate GC0308 detection, correct colors/orientation, server reception/inference, send spacing,
JPEG quality, CPU/heap use, and audio continuity. Exercise repeated conversation start/stop,
Wi-Fi loss, long-touch provisioning, bitrate reduction, and missing camera. Confirm no frames after
stop and stable memory after repeated cycles. Hardware and server acceptance must be recorded
separately. Validation of the PSRAM UI stack with concurrent video, full-duplex audio and UI,
including provisioning and volume-persistence NVS writes, remains pending. Compare `captured/window_ms`,
per-core CPU, internal free heap, and the largest free block while keeping the same scene and
audio activity. The expected reduction in DMA traffic is not a measured CPU reduction.
