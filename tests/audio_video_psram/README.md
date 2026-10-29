# Audio and video PSRAM lifecycle tests

Run `sh tests/audio_video_psram/run.sh`. Set `AUDIO_VIDEO_SANITIZERS=1` to enable
UndefinedBehaviorSanitizer. The tests compile the production PCM playback buffer
and CoreS3 video source with pthread-backed FreeRTOS, heap and camera mocks.

The fault-injection cases cover allocation and task-creation failures, partial
initialization cleanup, camera STREAMON failure, repeated start/stop, callbacks
that outlive a stop timeout, delayed worker suspension after completion, cleanup
retry, and resource release. The heap mock checks that FIFO, scratch, JPEG and
worker stacks use PSRAM; the task mock rejects self-deletion and deletion before
the worker reaches its suspended state. Timeout tests retain every resource
until the callback or delayed suspension completes. The clock is accelerated by
20 times to keep the production timeout paths affordable on the host.

These are host tests. They do not validate physical PSRAM/DMA behavior, camera
capture, audio quality, cache-disabled execution, or Agora server reception.
