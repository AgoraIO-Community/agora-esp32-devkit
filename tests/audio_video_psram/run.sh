#!/bin/sh
# SPDX-License-Identifier: Apache-2.0
set -eu
repo_root=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
test_dir="$repo_root/tests/audio_video_psram"
test_build_dir=$(mktemp -d /tmp/mybot-audio-video-psram.XXXXXX)
trap 'rm -r -- "$test_build_dir"' EXIT HUP INT TERM
sanitizer_flags=""
if [ "${AUDIO_VIDEO_SANITIZERS:-0}" = 1 ]; then
    sanitizer_flags="-fsanitize=undefined -fno-omit-frame-pointer"
fi
for test_name in pcm video; do
    "${CC:-cc}" -std=c11 -Wall -Wextra -Werror -pthread $sanitizer_flags \
        -D_POSIX_C_SOURCE=200809L -DCONFIG_MYBOT_DEBUG_RESOURCE_MONITOR=0 \
        -I"$test_dir/mocks" -I"$test_dir" \
        -I"$repo_root/components/mybot_platform/src/internal" \
        -I"$repo_root/components/mybot_stack/mybot_sdk/mybot/include" \
        "$test_dir/host_runtime.c" "$test_dir/test_$test_name.c" \
        -o "$test_build_dir/test_$test_name"
    python3 -c 'import subprocess,sys; subprocess.run(sys.argv[1:], check=True, timeout=30)' \
        "$test_build_dir/test_$test_name"
done
