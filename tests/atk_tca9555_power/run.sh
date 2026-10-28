#!/bin/sh
# SPDX-License-Identifier: MIT
set -eu

repo_root=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
test_dir="$repo_root/tests/atk_tca9555_power"
test_build_dir=$(mktemp -d)
trap 'rm -r -- "$test_build_dir"' EXIT HUP INT TERM

"${CC:-cc}" -std=c11 -Wall -Wextra -Werror \
    -I"$test_dir/mocks" \
    "$test_dir/test_atk_tca9555_power.c" \
    "$repo_root/components/mybot_platform/boards/atk-dnesp32s3/atk_hardware.c" \
    -o "$test_build_dir/test_atk_tca9555_power"

if command -v timeout >/dev/null 2>&1; then
    timeout 20 "$test_build_dir/test_atk_tca9555_power"
else
    "$test_build_dir/test_atk_tca9555_power"
fi
