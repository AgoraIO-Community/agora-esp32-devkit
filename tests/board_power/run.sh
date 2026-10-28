#!/bin/sh
# SPDX-License-Identifier: MIT
set -eu

repo_root=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
test_dir="$repo_root/tests/board_power"
test_build_dir=$(mktemp -d)
trap 'rm -r -- "$test_build_dir"' EXIT HUP INT TERM

run_variant() {
    name=$1
    define=$2
    source_file=$3
    "${CC:-cc}" -std=c11 -Wall -Wextra -Werror \
        -I"$test_dir/mocks" -D"$define=1" \
        "$test_dir/test_board_power.c" "$source_file" \
        -o "$test_build_dir/test-$name"
    if command -v timeout >/dev/null 2>&1; then
        timeout 20 "$test_build_dir/test-$name"
    else
        "$test_build_dir/test-$name"
    fi
}

run_variant sy6970 TEST_SY6970 \
    "$repo_root/components/mybot_platform/boards/lilygo-t-display-s3-pro-mvsrlora/lilygo_tdisplay_hardware.c"
run_variant atk-box TEST_ATK_BOX \
    "$repo_root/components/mybot_platform/boards/atk-dnesp32s3-box/atk_box_hardware.c"
run_variant tudouzi TEST_TUDOUZI \
    "$repo_root/components/mybot_platform/boards/tudouzi/tudouzi_hardware.c"
