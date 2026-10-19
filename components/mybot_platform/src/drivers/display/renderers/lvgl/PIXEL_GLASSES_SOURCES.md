# CoreS3 pixel-glasses renderer

This renderer adapts the BK7259 pixel-glasses view used by
[agora-r2-standard-devkit](https://github.com/AgoraIO-Community/agora-r2-standard-devkit)
for the existing CoreS3 320x240 LVGL display. The source's MIT copyright notices remain in
`pixel_glasses_view.cc` and `pixel_expression_shapes.h`. Other board profiles
retain the preceding shared `lvgl_view.cc`; this renderer does not initialize
panels, change GPIO assignments, rotate pixels, own DMA buffers, or create a
FreeRTOS task.

The `process` skin (工艺灰) and `geek` face (像素眼镜) are defined by the
delivered resources:

| Resource | Implementation |
| --- | --- |
| Layout, colors and drawing | [pixel_glasses_view.cc](pixel_glasses_view.cc) |
| Expression geometry | [pixel_expression_shapes.h](pixel_expression_shapes.h) |
| Immutable A8 masks | [pixel_expression_masks.h](pixel_expression_masks.h) |
| Mask geometry and verification | [generate-expression-masks.py](generate-expression-masks.py) |
| Terminal font data and generator | [pixel_fonts.c](pixel_fonts.c), [generate-pixel-font.py](generate-pixel-font.py) |

## Layout and data ownership

The renderer's 1024x896 design coordinates scale uniformly to a 288x252 artwork area at `(16,-6)`.
Only empty source margins are cropped. The black-glass background is fixed
`#050708`; neutral artwork uses `#f1f1ee`. Four terminal labels begin at
`(45,24)`, `(45,37)`, `(45,50)`, and `(45,63)`. Each is 194x12 pixels; the
longest normal state line `> state: in_conversation | vp:pending` measures
185 pixels. The first line identifies the target as `> MyBot / CoreS3`.
The 10-pixel `</>` signature starts at `(240,26)`.

The source rectangle for terminal width is widened from 650 to 688 units so
CoreS3 keeps all four rows single-line. Row spacing respects actual font line
height. The blinking cursor uses the same font's measured advance and the
actual fourth-row position.

Guide labels use x=36..283. Their title begins at y=92, pair code or SSID at
y=128, and notice at y=180. SSID and notice each reserve two 26-pixel lines,
leaving separate boxes through y=179 and y=231. Pair codes remain six-character
strings and always use Montserrat32; leading zeroes such as `000042` are
preserved. No length-based font substitution or numeric conversion occurs.

`create`/`create_sized`/`update`/`destroy` retain the existing view entry points.
The CoreS3 owner calls the additional `mybot_pixel_glasses_update` and
show/replay functions while holding its LVGL lock. Runtime state and SSID are
borrowed only for a call and copied into bounded view storage. Shape points
and both banks remain valid for the view's lifetime. The view owns one LVGL
timer, stopped and deleted before its widgets on destruction.

All 23 expressions are defined in `pixel_expression_shapes.h`. The platform's independent
expression policy owns SDK phases, opening expression and MCP hold/replay
timing; the renderer owns only their drawing. State/VP changes update the
third terminal row without restarting a selected expression. No reset,
motor-control or backend-protocol implementation is imported into the view.

## Masks and fonts

Five immutable A8 masks occupy 13,631 bitmap bytes: heart 51x48, star 77x74,
ellipse 49x59, cheek 67x29 and tear 21x31. `generate-expression-masks.py`
reconstructs their geometry at the original 9/16 sampling scale, using Pillow
10.2.0 and 4x coverage before downsampling. Public LVGL image drawing scales
them to the smaller artwork; there is no full-face canvas or image sequence.
Verify the committed bytes from the repository root with the read-only command below.
It requires Pillow 10.2.0 and `clang-format` and uses geometry encoded in the
generator:

```sh
python3 components/mybot_platform/src/drivers/display/renderers/lvgl/generate-expression-masks.py --check
```

`pixel_fonts.c` contains renamed MyBot Terminal subsets of external
`DejaVuSansMono-Bold.ttf`, pinned to SHA256
`3a3c502eeff669a231549e80df9f7c49de109bafe303170409e905d0b31a38fe`.
The 9-pixel ASCII subset uses 1,697 bitmap bytes; the 10-pixel `</>` subset
uses 60. Their total 1,757 bytes exclude glyph/descriptor metadata. The input
font stays external. Bitstream copyright, trademark notices, DejaVu notice
and the complete Bitstream Vera license are retained in
[TERMINAL_FONT_LICENSE.txt](TERMINAL_FONT_LICENSE.txt); the derived font name contains neither reserved
word. The public input font is DejaVu Fonts 2.37's `ttf/DejaVuSansMono-Bold.ttf`
from the [upstream release](https://github.com/dejavu-fonts/dejavu-fonts/releases/tag/version_2_37).
With a local copy matching the hash above, regenerate from the repository root with:

```sh
python3 components/mybot_platform/src/drivers/display/renderers/lvgl/generate-pixel-font.py \
  --font /path/to/DejaVuSansMono-Bold.ttf
```

Operational Chinese/English text reuses the existing 20-pixel MyBot UI Sans
font and its existing OFL provenance; this change does not regenerate that
shared font. All fixed visible glyphs are checked in the native rendering
test. Arbitrary Chinese SSIDs require broader font coverage and are not
established by checking the fixed vocabulary.

## Validation scope

Native LVGL 9.5 rendering validates both languages and disabled animations at
320x240, all 23 expressions and sampled transitions, four-line widths and
spacing, fixed codes `135790` and `000042`, 32-byte ASCII SSIDs, phase/VP text,
glyph availability, visible label bounds and repeated view destruction. These
checks use the ESP32 repository's LVGL sources with a host allocator.
They do not establish the physical panel's colors, byte order, touch behavior,
DMA timing, CPU load or concurrent RTC/video performance; those require a
CoreS3 firmware build and device validation.
