#!/usr/bin/env bash
# -------------------------------------------------------------
# run-clang-format.sh
#
# This file is part of the mio library.
#
# Copyright 2026 Maxtek Consulting (https://github.com/maxtek6/mio)
# Copyright 2017 mandreyel (https://github.com/mandreyel/mio)
#
# Licensed under the MIT License. See the LICENSE file.
# -------------------------------------------------------------
set -e

if ! command -v clang-format >/dev/null 2>&1; then
  echo "clang-format not found."
  exit 1
fi

find ../../ -type f \( -name "*.hpp" -o -name "*.cpp" \) \
    -exec clang-format -i -style=file {} +
