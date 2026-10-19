# CoreS3 Pixel-Glasses UI

[English](CORES3_UI.md) | [简体中文](CORES3_UI.zh-CN.md)

`MYBOT_BOARD=m5stack-core-s3` automatically selects the process-gray pixel-glasses view on the
320 × 240 ILI9342 display. It uses the existing CoreS3 LVGL adapter and common LCD-view API.
There is no manual renderer switch or `CONFIG_MYBOT_LVGL_UI` override. The other twelve board
profiles retain their existing UI and hardware behavior; see [Platform UI](PLATFORM_UI.md).

This replaces CoreS3's earlier status-card view. Audio, camera/video policy, FT6336 touch gestures,
power, pins, PSRAM and partitions retain their board configuration. Short touch still starts or
stops a conversation; a three-second hold still requests Wi-Fi provisioning. The agent-studio-device-firmware SDK,
AOSL and Agora RTSA snapshots are unchanged.

## Displayed information

The `process` skin and `geek` face use a fixed `#050708` black-glass background,
gray decorations and pixel glasses. Four terminal rows show the product label, selected mood,
runtime/session state and the realtime signature. The terminal uses a 9 px bitmap font and the
`</>` signature uses 10 px. During conversations, the state row includes listening/thinking/
speaking and `vp:ok` or `vp:pending`; voiceprint changes do not show a separate notification.
The artwork does not measure microphone amplitude or change the SDK runtime state.

All ten SDK workflow screens remain supported. Startup, provisioning, network loss, service
startup, pairing, pairing code, errors and shutdown show Chinese/English operational guidance.
Provisioning uses the actual active SoftAP SSID, with a generic setup label if unavailable.
Necessary overflowing SSID/instruction scrolling remains available when expression animations
are disabled and stops when its guidance page is left.

The pairing page directs users to the web console. A six-digit numeric pairing code uses a
fixed 32 px font on CoreS3; it does not shrink to fit. Chinese guidance reuses the existing
20 px Source Han Sans subset. That subset covers product UI text, not arbitrary Chinese chat.

## Automatic faces and optional LCD indicators

The public LCD interface retains four optional one-shot emotion indicators. The current SDK MCP
path accepts only `audio.set_volume` and `audio.adjust_volume` inside a `message.user` envelope;
it does not emit these emotion indicators. The table describes renderer behavior when indicators
are submitted, as exercised by the LCD adapter host tests.

| State or submitted LCD indicator | Face and hold |
| --- | --- |
| READY / return from a conversation | NEUTRAL |
| Enter a conversation | SURPRISED for 300 ms, plus entering transition |
| LISTENING | LISTENING |
| THINKING | THINKING; PROCESSING after three continuous seconds |
| SPEAKING / no active phase | NEUTRAL |
| `MYBOT_LCD_INDICATOR_HAPPY` | HAPPY for 2000 ms |
| `MYBOT_LCD_INDICATOR_SAD` | SAD for 3000 ms |
| `MYBOT_LCD_INDICATOR_ANGRY` | ANNOYED for 1200 ms |
| `MYBOT_LCD_INDICATOR_SURPRISED` | SURPRISED for 800 ms |

With animations enabled, each entering transition adds 250 ms before its hold, including the
conversation opener. With animations disabled, only the hold remains. Each submitted emotion
indicator replaces the unfinished foreground and replays even the same expression name.
Ordinary phase/voiceprint updates do not restart the hold.
Expiry restores the latest automatic face, including PROCESSING if thinking has continued for
three seconds in the background. Leaving a conversation or detaching the SDK cancels its
expression timers. Operational guidance takes precedence. All expressions are LCD-only and send
no motor commands.

The complete artwork set contains 23 names:

```text
NEUTRAL, GREETING, HAPPY, EXCITED, CURIOUS, CONFUSED, THINKING, LISTENING,
SURPRISED, SHY, PROUD, LOVE, SLEEPY, BORED, SAD, PLEADING, APOLOGY, WORRIED,
ANNOYED, REFUSE, PROCESSING, ERROR, SLEEP
```

GREETING and the other prepared artwork remain available without automatic demo cycling.
ERROR and SLEEP artwork do not enter an SDK error state or put the board to sleep. The CoreS3
adapter owns expression timing alongside its existing copied LCD-state submissions; rendering
stays in the existing LVGL task, without an extra UI thread or UTC dependency.

## Theme and animation settings

| Setting | CoreS3 behavior |
| --- | --- |
| `CONFIG_MYBOT_LVGL_UI_LIGHT_THEME` | Retained for saved-config compatibility; the pixel view always uses process gray |
| `CONFIG_MYBOT_LVGL_UI_ANIMATIONS` | Enables expression motion, 250 ms transitions and cursor animation; disabling it preserves state changes, hold expiry and necessary guidance scrolling |

The other display boards keep the shared workflow view's existing light/dark theme and activity
animation behavior. No new theme or expression-selection touch gesture is introduced.

## Build and flash

Use ESP-IDF v5.5.2. The normal builder selects CoreS3 and both languages, and its default
`--video auto` policy enables CoreS3 video through the existing board capability:

```sh
python3 scripts/build_all.py --board m5stack-core-s3 --language both \
  --build-root /tmp/mybot-cores3-pixel-build \
  --output-root /tmp/mybot-cores3-pixel-firmware
```

Build and firmware outputs stay in those dedicated, separate roots; they must not contain each
other, `..` path components, or symbolic-link components. On macOS use `/private/tmp/` instead of
the `/tmp/` alias. Repository-local roots must stay within `build/` or `releases/`.

The script marks new or empty roots with `.mybot-generated-root.json` and cleans them by default.
Existing nonempty roots without a matching marker are refused, even with `--no-clean`. Inspect and
move older directories aside or choose fresh roots; do not create markers manually. `--no-clean`
preserves data while still checking ownership. `--dry-run` validates the same restrictions without
creating directories or markers. Cleanup requires POSIX safe directory handling and Python 3.11+
and is refused otherwise. Use `--video off` for an audio/UI comparison; these options do not select
another renderer.

A direct IDF build uses its selected sdkconfig/defaults, without the builder's automatic video
addition. For example:

```sh
idf.py -B build/cores3-pixel \
  -DMYBOT_BOARD=m5stack-core-s3 \
  -DSDKCONFIG=build/cores3-pixel/sdkconfig \
  -DSDKCONFIG_DEFAULTS=sdkconfig.defaults build
idf.py -B build/cores3-pixel -p <PORT> flash monitor
```

Use separate sdkconfigs for language, video and animation comparisons; defaults do not overwrite
saved configuration values. Build validation is recorded separately below.

## Rendering, resources and provenance

The existing LVGL task runs on Core 1 at priority 1. Objects and drawing scratch space use PSRAM;
SPI transfers retain a 320 × 16 RGB565 stripe, or 10,240 bytes of internal DMA memory. The UI task
stack and driver/synchronization allocations are additional memory. Partial redraws avoid a
full-screen page cache or decoded full-face buffer.

Fixed LVGL widgets and curves draw the face. Five immutable A8 masks contribute 13,631 alpha
bytes for hearts, stars, ellipses, cheeks and tears, excluding descriptor overhead. The terminal
font is a renamed DejaVu Sans Mono Bold subset under its retained Bitstream Vera terms; the
Source Han Sans UI subset retains SIL OFL 1.1. The masks and terminal fonts are compiled only for
the CoreS3 pixel renderer; other boards retain their existing state-card images.

The display implementation and generators derive from `github.com/junlon2006/bk_solution_ai`, commit
`dc4c85492bcf00f780c92d56c6cba2a362f54f68`, preserving file-level MIT/Apache-2.0 notices and the
existing display license. The delivered [renderer and resource guide](../components/mybot_platform/src/drivers/display/renderers/lvgl/PIXEL_GLASSES_SOURCES.md)
links the geometry, colors, masks and generators and provides a read-only resource check.
The corresponding official firmware project is
[agora-r2-standard-devkit](https://github.com/AgoraIO-Community/agora-r2-standard-devkit).
See [dependency baselines](../VENDORED_SOURCES.md) and [third-party notices](../THIRD_PARTY_NOTICES.md).

## Validation status

The first Chinese and English CoreS3 builds passed with ESP-IDF v5.5.2 through the standard
builder, using automatic video and 60 ms audio packet time: 38 s for zh-CN, 37 s for en-US,
1 min 16 s for the batch. This is build validation, not a hardware result. Host previews and
device regression remain separate checks.

Earlier CoreS3 hardware tests cover the previous status-card UI and established audio/camera/
input paths. They do not validate this new pixel renderer. Real-device testing remains required.

- Check all ten guidance screens and 23 expressions in both languages on the 320 × 240 panel.
- Verify four separate terminal rows, the signature and full-size six-digit pairing codes.
- Verify the SURPRISED opener, phase changes and three-second thinking timeout. Inject optional
  LCD emotion indicators to test replacement, same-indicator replay and expiry; phase/voiceprint
  updates must preserve hold timing. The SDK MCP path cannot trigger these expression tests.
- Test animation enabled/disabled, overflowing SSID/instructions, and web-console pairing.
- Regress short touch, three-second provisioning, Wi-Fi recovery and repeated SDK stop/start.
- Run full-duplex audio with video off and with the existing 1 fps camera uplink enabled.
- Record CPU load, internal/PSRAM peaks, minimum free heap and largest blocks; repeat sessions
  to check resource lifetime and sustained memory growth.

Include board, firmware language, video/animation settings, volume and serial logs with results.
