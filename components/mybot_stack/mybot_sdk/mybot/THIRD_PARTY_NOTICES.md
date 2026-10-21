# Third-party notices

The [SDK Apache-2.0 license](LICENSE) applies to code maintained by agent-studio-device-firmware
unless a file states otherwise. It does not replace third-party terms. This file is informational,
not legal advice.

## AOSL

Location: [components/mybot_stack/aosl](../../aosl) — bundled sources pinned to upstream commit
`84e086084ebcd0ae2455a0ce5721950c5fe2e656` of https://github.com/AgoraIO-Community/aosl.

AOSL includes its [LICENSE](../../aosl/LICENSE), which is based on Apache-2.0 and adds restrictive
conditions. Read that file before using, modifying, deploying, or redistributing AOSL. Do not label
the combined repository or binary as uniformly Apache-2.0.

The ESP32 firmware repository includes these sources directly. See its
[release guide](../../../../docs/RELEASING.md) for dependency and artifact checks.

## Agora RTSA SDK

Location: [components/mybot_stack/agora_rtc](../../agora_rtc). This firmware uses the
ESP32-S3 static RTSA Lite package, v1.10.1, build 1270872 (`20260828_194128`), recorded in
[dependency baselines](../../../../VENDORED_SOURCES.md).

No standalone license or NOTICE for the bundled RTSA binary was found in the supplied package.
Possession of the files is not evidence of redistribution rights. Commercial or
production use and redistribution (source archives, binaries, container/firmware images, mirrors)
require a separate license from Agora (声网). Release maintainers must retain the applicable terms
and written redistribution authorization in the release records before publishing an artifact that
contains the binary; the presence of this repository copy does not itself record that authorization.

File-specific copyright and license headers also remain in force.

## cJSON-derived implementation

`src/support/mybot_json.c` and `src/internal/mybot_json.h` are namespaced derivatives of cJSON.
They retain the MIT license and Dave Gamble copyright notice in the source.

## Release requirement

Publish a release artifact containing the bundled Agora binary only after its license and written
redistribution authorization have been verified and retained in the release records. If that record
is unavailable, exclude the binary and require users to supply an authorized package, as described
in the firmware's [third-party notices](../../../../THIRD_PARTY_NOTICES.md#agora-rtsa).
