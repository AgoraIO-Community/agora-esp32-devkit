#!/bin/sh
# SPDX-License-Identifier: MIT
set -eu

repo_root=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
test_dir="$repo_root/tests/compact_panel"
mock_dir="$repo_root/tests/st7789_lifecycle/mocks"
board_dir="$repo_root/components/mybot_platform/boards/magiclick-2p5"
test_build_dir=$(mktemp -d)
trap 'rm -r -- "$test_build_dir"' EXIT HUP INT TERM

"${CC:-cc}" -std=c11 -Wall -Wextra -Werror \
    -I"$mock_dir" -I"$board_dir" \
    -I"$repo_root/components/mybot_platform/src/internal" \
    -c "$repo_root/components/mybot_platform/src/drivers/display/panels/compact/compact_128_panel.c" \
    -o "$test_build_dir/compact_128_panel.o"
"${CXX:-c++}" -std=c++17 -Wall -Wextra -Werror \
    -I"$mock_dir" -I"$board_dir" \
    -I"$repo_root/components/mybot_platform/src/internal" \
    "$test_dir/test_compact_panel.cc" "$test_build_dir/compact_128_panel.o" \
    -o "$test_build_dir/test_compact_panel"

if command -v timeout >/dev/null 2>&1; then
    timeout 20 "$test_build_dir/test_compact_panel"
else
    "$test_build_dir/test_compact_panel"
fi
