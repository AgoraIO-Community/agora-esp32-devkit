#!/bin/sh
# SPDX-License-Identifier: MIT
set -eu

repo_root=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
test_dir="$repo_root/tests/aw9523b"
test_build_dir=$(mktemp -d)
trap 'rm -r -- "$test_build_dir"' EXIT HUP INT TERM

"${CC:-cc}" -std=c11 -Wall -Wextra -Werror \
    -I"$test_dir/mocks" \
    -I"$repo_root/components/mybot_platform/src/internal" \
    "$test_dir/test_aw9523b.c" \
    "$repo_root/components/mybot_platform/src/drivers/io_expander/aw9523b.c" \
    -o "$test_build_dir/test_aw9523b"

if command -v timeout >/dev/null 2>&1; then
    timeout 20 "$test_build_dir/test_aw9523b"
else
    "$test_build_dir/test_aw9523b"
fi
