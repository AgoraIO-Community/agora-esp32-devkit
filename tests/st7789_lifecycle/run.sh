#!/bin/sh
# SPDX-License-Identifier: MIT
set -eu

repo_root=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
test_dir="$repo_root/tests/st7789_lifecycle"
test_build_dir=$(mktemp -d)
trap 'rm -r -- "$test_build_dir"' EXIT HUP INT TERM

for board_id in xingzhi-cube-1.54tft-wifi genjutech-s3-1.54tft; do
    board_dir="$repo_root/components/mybot_platform/boards/$board_id"
    "${CC:-cc}" -std=c11 -Wall -Wextra -Werror \
        -I"$test_dir/mocks" -I"$board_dir" \
        -I"$repo_root/components/mybot_platform/src/internal" \
        -c "$repo_root/components/mybot_platform/src/drivers/display/panels/st7789/st7789_panel.c" \
        -o "$test_build_dir/panel-$board_id.o"
    "${CXX:-c++}" -std=c++17 -Wall -Wextra -Werror -pthread \
        -I"$test_dir/mocks" -I"$board_dir" \
        -I"$repo_root/components/mybot_platform/src/internal" \
        -I"$repo_root/components/mybot_stack/mybot_sdk/mybot/include" \
        "$test_dir/test_st7789_lifecycle.cc" \
        "$repo_root/components/mybot_platform/src/drivers/display/adapters/lvgl/shared_lvgl_adapter.cc" \
        "$test_build_dir/panel-$board_id.o" -o "$test_build_dir/test_st7789_lifecycle-$board_id"
    if command -v timeout >/dev/null 2>&1; then
        timeout 20 "$test_build_dir/test_st7789_lifecycle-$board_id"
    else
        "$test_build_dir/test_st7789_lifecycle-$board_id"
    fi
done

board_dir="$repo_root/components/mybot_platform/boards/xingzhi-cube-1.54tft-wifi"
"${CC:-cc}" -std=c11 -Wall -Wextra -Werror \
    -I"$test_dir/mocks" -I"$board_dir" \
    -I"$repo_root/components/mybot_platform/include" \
    -I"$repo_root/components/mybot_platform/src/internal" \
    -I"$repo_root/components/mybot_stack/mybot_sdk/mybot/include" \
    -c "$board_dir/board.c" -o "$test_build_dir/board.o"
"${CXX:-c++}" -std=c++17 -Wall -Wextra -Werror -pthread \
    -I"$test_dir/mocks" -I"$board_dir" \
    -I"$repo_root/components/mybot_platform/include" \
    -I"$repo_root/components/mybot_platform/src/internal" \
    -I"$repo_root/components/mybot_stack/mybot_sdk/mybot/include" \
    "$test_dir/test_xingzhi_board.cc" "$test_build_dir/board.o" \
    -o "$test_build_dir/test_xingzhi_board"

for scenario in rtc_hold_dis rtc_deinit gpio_config gpio_high ledc_timer ledc_channel \
    ledc_duty ledc_update lcd_init lcd_render buttons_start ok; do
    "$test_build_dir/test_xingzhi_board" "$scenario"
done
