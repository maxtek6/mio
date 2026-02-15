#!/usr/bin/env bash
# -------------------------------------------------------------
# run-tests.sh
#
# This file is part of the mio library.
#
# Copyright 2026 Maxtek Consulting (https://github.com/maxtek6/mio)
# Copyright 2017 mandreyel (https://github.com/mandreyel/mio)
#
# Licensed under the MIT License. See the LICENSE file.
# -------------------------------------------------------------
set -e

if ! command -v ctest >/dev/null 2>&1; then
  echo "ctest not found."
  exit 1
fi

ctest -C Debug --output-on-failure
