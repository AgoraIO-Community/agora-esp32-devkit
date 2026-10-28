#!/bin/sh
# SPDX-License-Identifier: MIT
set -eu

repo_root=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
test_dir="$repo_root/tests/cst226_input"
board_dir="$repo_root/components/mybot_platform/boards/lilygo-t-display-s3-pro-mvsrlora"
test_build_dir=$(mktemp -d)
trap 'rm -r -- "$test_build_dir"' EXIT HUP INT TERM

"${CC:-cc}" -std=c11 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Werror -pthread \
    -I"$repo_root/tests/cst816s_input/mocks" -I"$board_dir" \
    "$test_dir/test_cst226_input.c" \
    "$repo_root/components/mybot_platform/src/drivers/input/cst816s_boot_input.c" \
    -o "$test_build_dir/test_cst226_input"

if command -v timeout >/dev/null 2>&1; then
    timeout 20 "$test_build_dir/test_cst226_input"
else
    "$test_build_dir/test_cst226_input"
fi
