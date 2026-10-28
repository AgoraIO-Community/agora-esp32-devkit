#!/bin/sh
# SPDX-License-Identifier: MIT
set -eu

repo_root=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
test_dir="$repo_root/tests/i80_st7789_lifecycle"
mock_dir="$repo_root/tests/st7789_lifecycle/mocks"
board_dir="$repo_root/components/mybot_platform/boards/atk-dnesp32s3-box"
test_build_dir=$(mktemp -d)
trap 'rm -r -- "$test_build_dir"' EXIT HUP INT TERM

"${CC:-cc}" -std=c11 -Wall -Wextra -Werror \
    -I"$mock_dir" -I"$board_dir" \
    -I"$repo_root/components/mybot_platform/src/internal" \
    -c "$repo_root/components/mybot_platform/src/drivers/display/panels/generic/i80_st7789_panel.c" \
    -o "$test_build_dir/i80_st7789_panel.o"
"${CXX:-c++}" -std=c++17 -Wall -Wextra -Werror \
    -I"$mock_dir" -I"$board_dir" \
    -I"$repo_root/components/mybot_platform/src/internal" \
    "$test_dir/test_i80_st7789_lifecycle.cc" "$test_build_dir/i80_st7789_panel.o" \
    -o "$test_build_dir/test_i80_st7789_lifecycle"

if command -v timeout >/dev/null 2>&1; then
    timeout 20 "$test_build_dir/test_i80_st7789_lifecycle"
else
    "$test_build_dir/test_i80_st7789_lifecycle"
fi
