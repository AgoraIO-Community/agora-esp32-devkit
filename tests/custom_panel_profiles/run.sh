#!/bin/sh
# SPDX-License-Identifier: MIT
set -eu

repo_root=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
test_dir="$repo_root/tests/custom_panel_profiles"
mock_dir="$repo_root/tests/st7789_lifecycle/mocks"
test_build_dir=$(mktemp -d)
trap 'rm -r -- "$test_build_dir"' EXIT HUP INT TERM

run_board() {
    board=$1
    controller=$2
    board_dir="$repo_root/components/mybot_platform/boards/$board"
    "${CC:-cc}" -std=c11 -Wall -Wextra -Werror \
        -I"$mock_dir" -I"$board_dir" \
        -I"$repo_root/components/mybot_platform/src/internal" \
        -c "$repo_root/components/mybot_platform/src/drivers/display/panels/generic/rgb565_spi_panel.c" \
        -o "$test_build_dir/$board-panel.o"
    "${CXX:-c++}" -std=c++17 -Wall -Wextra -Werror \
        -DEXPECTED_CONTROLLER="$controller" \
        -I"$mock_dir" -I"$board_dir" \
        -I"$repo_root/components/mybot_platform/src/internal" \
        "$test_dir/test_custom_panel.cc" "$test_build_dir/$board-panel.o" \
        -o "$test_build_dir/test-$board"
    if command -v timeout >/dev/null 2>&1; then
        timeout 20 "$test_build_dir/test-$board"
    else
        "$test_build_dir/test-$board"
    fi
}

run_board lilygo-t-circle-s3 4
run_board jiuchuan-s3 5
