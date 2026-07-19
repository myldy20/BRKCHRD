#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu

if [ "$#" -ne 2 ]; then
  echo "usage: $0 BUILD_DIR OUTPUT_DIR" >&2
  exit 2
fi

BUILD_DIR=$(CDPATH= cd -- "$1" && pwd)
mkdir -p "$2"
OUTPUT_DIR=$(CDPATH= cd -- "$2" && pwd)
ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
ASSETS="$ROOT_DIR/packaging/portmaster/brkchrd"
STAGE="$OUTPUT_DIR/brkchrd-stage"
PACKAGE="$STAGE/brkchrd"

[ -x "$BUILD_DIR/brkchrd-sdl" ] || { echo "missing binary: $BUILD_DIR/brkchrd-sdl" >&2; exit 1; }
rm -rf "$STAGE"
mkdir -p "$PACKAGE/brkchrd/docs" "$PACKAGE/brkchrd/licenses" "$PACKAGE/brkchrd/conf"
cp "$ASSETS/port.json" "$PACKAGE/port.json"
cp "$ASSETS/gameinfo.xml" "$PACKAGE/gameinfo.xml"
cp "$ASSETS/README.md" "$PACKAGE/README.md"
cp "$ASSETS/BRKCHRD.sh" "$PACKAGE/BRKCHRD.sh"
cp "$BUILD_DIR/brkchrd-sdl" "$PACKAGE/brkchrd/brkchrd-sdl.aarch64"
cp "$ROOT_DIR"/docs/*.md "$PACKAGE/brkchrd/docs/"
cp "$ROOT_DIR/LICENSE" "$PACKAGE/brkchrd/licenses/GPL-3.0.txt"
cp "$ROOT_DIR/THIRD_PARTY_NOTICES.md" "$PACKAGE/brkchrd/licenses/THIRD_PARTY_NOTICES.md"
chmod +x "$PACKAGE/BRKCHRD.sh" "$PACKAGE/brkchrd/brkchrd-sdl.aarch64"
(cd "$PACKAGE" && zip -9 -r "$OUTPUT_DIR/brkchrd-v0.3.0-portmaster.zip" .)
rm -rf "$STAGE"
echo "$OUTPUT_DIR/brkchrd-v0.3.0-portmaster.zip"
