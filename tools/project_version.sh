#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu

ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
VERSION=$(sed -n 's/^project(brkchrd VERSION \([0-9][0-9.]*\) LANGUAGES CXX)$/\1/p' "$ROOT_DIR/CMakeLists.txt")

if [ -z "$VERSION" ]; then
  echo "unable to read BRKCHRD version from CMakeLists.txt" >&2
  exit 1
fi

printf '%s\n' "$VERSION"
