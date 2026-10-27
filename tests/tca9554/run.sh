#!/bin/sh
# SPDX-License-Identifier: MIT
set -eu

repo_root=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
test_dir="$repo_root/tests/tca9554"
test_build_dir=$(mktemp -d)
trap 'rm -r -- "$test_build_dir"' EXIT HUP INT TERM

"${CC:-cc}" -std=c11 -Wall -Wextra -Werror \
    -I"$test_dir/mocks" \
    -I"$repo_root/components/mybot_platform/src/internal" \
    "$test_dir/test_tca9554.c" \
    "$repo_root/components/mybot_platform/src/drivers/io_expander/tca9554.c" \
    -o "$test_build_dir/test_tca9554"

if command -v timeout >/dev/null 2>&1; then
    timeout 20 "$test_build_dir/test_tca9554"
else
    "$test_build_dir/test_tca9554"
fi
