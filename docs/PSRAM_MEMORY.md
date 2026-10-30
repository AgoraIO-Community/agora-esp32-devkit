# PSRAM memory policy

[English](PSRAM_MEMORY.md) | [简体中文](PSRAM_MEMORY.zh-CN.md)

Platform data and worker stacks use PSRAM when their access and lifecycle allow it. Internal
RAM remains available for hardware DMA, kernel objects and flash operations. This policy covers
application/platform code; the immutable agent-studio-device-firmware, AOSL and RTSA snapshots are
unchanged.
Build and real-device results are tracked only in the [validation matrix](VALIDATION.md).

## Configuration and allocation

The hidden `CONFIG_MYBOT_PSRAM_ALLOCATIONS` policy requires initialized, heap-accessible PSRAM
and selects `CONFIG_FREERTOS_TASK_CREATE_ALLOW_EXT_MEM` and
`CONFIG_NVS_ALLOCATE_CACHE_IN_SPIRAM`. Platform CMake rejects a configuration without the
policy or external-stack support. Reconfigure an existing build directory after updating.

Migrated data and stack allocations explicitly request `MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT`. Failure
returns through the existing initialization/cleanup path instead of consuming internal DMA
headroom. This includes the PCM FIFO; it no longer falls back to internal RAM. Driver-owned
allocations retain the ESP-IDF driver's capabilities and error handling.

ESP32-S3 target defaults already select `CONFIG_MBEDTLS_EXTERNAL_MEM_ALLOC=y`, a 2,048-byte
default-malloc internal preference threshold, and a 98,304-byte internal reserve. TLS workspace
was already in PSRAM; moving the small HTTPS connection wrapper adds no comparable TLS saving.
The reserve serves explicit internal/DMA requests and does not guarantee one contiguous block.

NVS uses ESP-IDF's supported PSRAM-preferred allocator with internal fallback for cache structures
and key hash lists. Savings depend on initialized partition size and stored entries: the usual
8/16-MiB layouts contain only a 16-KiB NVS partition, while SenseCAP's normal NVS partition is
840 KiB. The separate 200-KiB factory partition is not initialized by this application. Do not
estimate RAM savings from the board's entire Flash capacity. ESP-IDF documents approximately
2.5 times slower integer NVS operations with PSRAM cache; measure persistence and provisioning.

## Allocation inventory

Sizes below describe allocations, not measured reductions in internal heap. Some buffers were
already in PSRAM, and default malloc could previously select either region. Per-board rows are
alternatives; task control blocks and allocator overhead are additional memory.

| Allocation | PSRAM capacity and lifetime |
| --- | --- |
| All display adapters' LVGL stack | 7,168 bytes while the display port is active; CoreS3 was already configured this way |
| CoreS3 pixel view | `sizeof(View)`, including both shape banks and retained label strings; freed after LVGL widgets are deleted |
| PCM writer | 4,096-byte stack, 7,680-byte FIFO and separate 480-byte PCM scratch per playback context |
| Codec stream data | Stream contexts and scratch in PSRAM: CoreS3 capture 1,920 bytes; ES8311/AMOLED175/VoCat/SenseCAP scratch 3,840 bytes per stream; XVF3800 scratch 7,680 bytes per stream |
| Raw I2S conversion scratch | 3,840 or 7,680 bytes per context according to the board's slot configuration; its atomic control context stays internal |
| ES8374 codec control | `sizeof(audio_codec_es8374_t)` while the codec instance exists |
| CoreS3 offline audio comparison | 1,920-byte playback frame and optional 1,920-byte capture frame; decoded source PCM also uses PSRAM |
| CoreS3 JPEG worker | 6,144-byte stack while a worker exists; the existing two 153,600-byte capture buffers and 131,072-byte JPEG output also use PSRAM |
| FT6336, AMOLED175, VoCat and SenseCAP polling tasks | 3,072-byte stack for the selected board's input task |
| Shared CST816/FT5x06/GT911/CST226 input and XVF3800 button tasks | 4,096-byte stack for the selected board's input task |
| Optional resource monitor | 3,072-byte stack when `CONFIG_MYBOT_DEBUG_RESOURCE_MONITOR=y` |
| Announcement and HTTPS wrappers | Small application contexts explicitly in PSRAM; Ogg packet/decoded PCM buffers and TLS workspace already use PSRAM |

All display boards use a separate internal RGB565 DMA stripe of `width × 16 × 2` bytes.
CoreS3's stripe is 10,240 bytes. Its camera also needs a contiguous 7,680-byte internal DVP DMA
buffer with the current 8,192-byte configuration bound. Free PSRAM cannot satisfy either DMA
request. Kernel TCBs, static semaphores, locks, ISR data, and the PCM buffer's synchronization
context stay internal. The 16-KiB main task stack stays internal because startup and provisioning
invoke NVS/Flash operations that disable cache. Vendor capture workers retain their own policy.

## Worker ownership and cleanup

PSRAM stacks use ESP-IDF `xTaskCreateWithCaps`, or the LVGL port's `task_stack_caps`; TCBs stay
internal. A stoppable application worker completes I/O and callbacks, publishes completion, then
suspends. Its owner retains the handle, waits for the suspended state, and calls
`vTaskDeleteWithCaps` before freeing resources. This avoids allocating an internal helper task
for self-deletion. Completion remains available after a timeout so cleanup can be retried.

PCM, video and stoppable input cleanup retain resources when joining or draining callbacks times
out. A retry finishes cleanup before another start can reuse the context. Process-lifetime input
and monitor tasks keep their PSRAM stacks until reboot. LVGL worker cleanup remains owned by its
port. Never move a task that performs flash writes to PSRAM without auditing the entire call path.

## Verification commands and device regression

With ESP-IDF 6.1 active, a fresh CoreS3 development build can use:

```sh
idf.py -B build/cores3-psram \
  -DMYBOT_BOARD=m5stack-core-s3 \
  -DSDKCONFIG=build/cores3-psram/sdkconfig \
  -DSDKCONFIG_DEFAULTS="sdkconfig.defaults;ci/ptime60.defaults;ci/video.defaults" build
```

For an existing directory, use `idf.py -B <build-dir> reconfigure build`. Inspect the generated
policy, external-stack and NVS-cache symbols; display profiles must also retain
`CONFIG_MYBOT_LVGL_UI=y`. These host checks cover allocation failure, partial cleanup, timeouts,
callback ordering and repeated start/stop:

```sh
sh tests/audio_video_psram/run.sh
AUDIO_VIDEO_SANITIZERS=1 sh tests/audio_video_psram/run.sh
sh tests/cst816s_input/run.sh
sh tests/cst226_input/run.sh
sh tests/st7789_lifecycle/run.sh
sh tests/cores3_pixel_ui/run.sh
git diff --check
```

On each affected device, verify provisioning/reconnect and HTTPS pairing, NVS volume save and
reboot restore, repeated SDK/audio/video start-stop, callback shutdown, touch/long press, display
orientation and redraw, finite prompt drain, and concurrent full-duplex audio plus video/UI.
Record per-core CPU, internal/PSRAM minimum free heap and largest DMA block before stream-on and
across repeated cycles. Check capture cadence and server reception separately from local frame
submission. Record new physical results in [VALIDATION.md](VALIDATION.md); builds and host tests
do not establish cache safety, audio continuity or hardware behavior.
