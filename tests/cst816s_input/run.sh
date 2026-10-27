#!/bin/sh
# SPDX-License-Identifier: MIT
set -eu

repo_root=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
test_dir="$repo_root/tests/cst816s_input"
test_build_dir=$(mktemp -d)
trap 'rm -r -- "$test_build_dir"' EXIT HUP INT TERM

"${CC:-cc}" -std=c11 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Werror -pthread \
    -I"$test_dir/mocks" \
    -I"$repo_root/components/mybot_platform/boards/esp32-s3-touch-lcd-1.85" \
    "$test_dir/test_cst816s_input.c" \
    "$repo_root/components/mybot_platform/src/drivers/input/cst816s_boot_input.c" \
    -o "$test_build_dir/test_cst816s_input"

if command -v timeout >/dev/null 2>&1; then
    timeout 20 "$test_build_dir/test_cst816s_input"
else
    "$test_build_dir/test_cst816s_input"
fi
