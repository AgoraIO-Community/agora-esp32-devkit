# Embedded assets

## Announcement audio

The `locales/zh-CN` and `locales/en-US` directories contain the Wi-Fi provisioning prompt, the
pairing prompt, and spoken digits `0` through `9`.

Only the locale selected by `CONFIG_MYBOT_LANGUAGE_*` is embedded in a firmware image. The audio is
decoded to 16 kHz mono signed-16 PCM in PSRAM when the SDK opens an announcement sound.

The assets use 20 ms Opus packets. Replacements must retain that packet duration, a mono
OpusHead stream, and a valid final granule position so playback can trim pre-skip and end padding.

The 22 pairing prompt and digit recordings are converted from the mybot SDK's 16 kHz mono signed-16
little-endian PCM at commit `7854c1d5798004485aa1947ff55116bdb7ab0237`. They use the SDK's
[Apache-2.0 license](../../mybot_stack/mybot_sdk/mybot/LICENSE).
[locales/SOURCES.json](locales/SOURCES.json) records the source revision, source/output SHA-256,
sample counts, encoder version and full encoding command.

The local `wificonfig.ogg` in each locale is played when Wi-Fi provisioning starts. Its format,
checksums and decoded frame counts are recorded under `wifi_provisioning` in `locales/SOURCES.json`.

To regenerate a pairing asset, obtain `assets/locales/<locale>/<sound>.pcm` from the pinned SDK
commit and use FFmpeg 6.1.1 with libopus 1.4. Run the command below for each locale (`en-US`, `zh-CN`)
and sound (`prompt`, `0` through `9`), substituting the input and output paths:

```sh
ffmpeg -nostdin -hide_banner -loglevel error \
  -f s16le -ar 16000 -ac 1 -i input.pcm -map_metadata -1 \
  -c:a libopus -b:a 24000 -vbr off -compression_level 10 \
  -application voip -frame_duration 20 -flags:a +bitexact -fflags +bitexact \
  -f ogg -y output.ogg
```

Firmware builds embed the checked-in Ogg files directly; FFmpeg and the PCM files are not build
dependencies. Host decoding checks do not establish real-device playback quality.

## CoreS3 UI images

`ui/noto_emoji` contains four local state images: neutral, happy, relaxed, and thinking. The
original 64x61 PNG files are retained as reproducible inputs. The
[asset generator](../../../scripts/generate-lvgl-ui-assets.py) pads them to 64x64 and produces
constant ARGB8888 arrays in `src/drivers/display/renderers/lvgl/lvgl_assets.c`. Firmware builds use those
arrays only when an LVGL UI option is enabled; no runtime PNG decoder is needed.

These rasterized Noto Color Emoji font glyphs retain the font's
[SIL Open Font License 1.1](ui/noto_emoji/LICENSE.txt).
[SOURCES.json](ui/noto_emoji/SOURCES.json) records the pinned source package, original font
provenance, per-file SHA-256 values, and conversion details. Run the generator with `--check`
to verify that the source hashes and generated arrays match.
