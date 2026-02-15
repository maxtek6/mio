#!/usr/bin/env bash
# -------------------------------------------------------------
# run-cppcheck.sh
#
# This file is part of the mio library.
#
# Copyright 2026 Maxtek Consulting (https://github.com/maxtek6/mio)
# Copyright 2017 mandreyel (https://github.com/mandreyel/mio)
#
# Licensed under the MIT License. See the LICENSE file.
# -------------------------------------------------------------

set -e

if ! command -v cppcheck >/dev/null 2>&1; then
  echo "cppcheck not found."
  exit 1
fi

cppcheck \
    --enable=all \
    --inconclusive \
    --std=c++11 \
    --suppress=missingIncludeSystem \
    --suppress=missingInclude \
    --enable=style \
    --enable=performance \
    --suppress=missingSystemInclude \
    --suppress=unmatchedSuppression \
    --error-exitcode=1 \
    --inline-suppr \
    ./include/** \
    ./example/*.cpp
