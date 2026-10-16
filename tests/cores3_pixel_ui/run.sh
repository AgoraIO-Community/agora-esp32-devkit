#!/bin/sh
# SPDX-License-Identifier: MIT
set -eu
repo_root=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
test_dir="$repo_root/tests/cores3_pixel_ui"
test_build_dir=$(mktemp -d /tmp/mybot-cores3-ui-test.XXXXXX)
trap 'rm -r -- "$test_build_dir"' EXIT HUP INT TERM
sanitizer_flags=""
if [ "${CORES3_PIXEL_UI_SANITIZERS:-0}" = 1 ]; then
    sanitizer_flags="-fsanitize=address,undefined -fno-omit-frame-pointer"
fi
"${CXX:-c++}" -std=c++17 -Wall -Wextra -Werror -pthread \
    $sanitizer_flags \
    -DMYBOT_CORES3_PIXEL_UI=1 -DCONFIG_MYBOT_LVGL_UI_ANIMATIONS=1 \
    -I"$test_dir/mocks" -I"$repo_root/tests/st7789_lifecycle/mocks" \
    -I"$repo_root/components/mybot_platform/include" \
    -I"$repo_root/components/mybot_platform/src/internal" \
    -I"$repo_root/components/mybot_stack/mybot_sdk/mybot/include" \
    "$test_dir/test_adapter.cc" \
    "$repo_root/components/mybot_platform/src/drivers/display/adapters/lvgl/ili9342_lvgl_adapter.cc" \
    -o "$test_build_dir/test_adapter"
timeout 20 "$test_build_dir/test_adapter"
