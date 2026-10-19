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

## xiaozhi-esp32 announcement assets

The firmware's [announcement assets](../../../../components/mybot_platform/assets/locales)
derive from the xiaozhi-esp32 project
(https://github.com/78/xiaozhi-esp32), which is distributed under the MIT License
(c) 2025 Shenzhen Xinzhi Future Technology Co., Ltd. and Project Contributors. The pairing-code
prompt and digit sounds retain the full MIT notice below, which must accompany any redistribution
of these files.

    MIT License

    Copyright (c) 2025 Shenzhen Xinzhi Future Technology Co., Ltd.
    Copyright (c) 2025 Project Contributors

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.

The firmware also retains the MIT notice in
[LICENSE.xiaozhi-esp32](../../../../components/mybot_platform/assets/LICENSE.xiaozhi-esp32).

## Release requirement

Publish a release artifact containing the bundled Agora binary only after its license and written
redistribution authorization have been verified and retained in the release records. If that record
is unavailable, exclude the binary and require users to supply an authorized package, as described
in the firmware's [third-party notices](../../../../THIRD_PARTY_NOTICES.md#agora-rtsa).
