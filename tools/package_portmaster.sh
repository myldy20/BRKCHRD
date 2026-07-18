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
PACKAGE_ROOT="$STAGE/brkchrd"

if [ ! -x "$BUILD_DIR/brkchrd-sdl" ]; then
    echo "missing binary: $BUILD_DIR/brkchrd-sdl" >&2
    exit 1
fi

rm -rf "$STAGE"
mkdir -p "$PACKAGE_ROOT/brkchrd/licenses" "$PACKAGE_ROOT/brkchrd/docs"
cp "$ASSETS/port.json" "$PACKAGE_ROOT/port.json"
cp "$ASSETS/gameinfo.xml" "$PACKAGE_ROOT/gameinfo.xml"
cp "$ASSETS/README.md" "$PACKAGE_ROOT/README.md"
cp "$ASSETS/BRKCHRD.sh" "$PACKAGE_ROOT/BRKCHRD.sh"
cp "$ROOT_DIR/README.md" "$PACKAGE_ROOT/brkchrd/docs/README.md"
cp "$ROOT_DIR/docs/research-and-workflow.md" "$PACKAGE_ROOT/brkchrd/docs/research-and-workflow.md"
cp "$ROOT_DIR/LICENSE" "$PACKAGE_ROOT/brkchrd/licenses/GPL-3.0.txt"
cp "$ROOT_DIR/THIRD_PARTY_NOTICES.md" "$PACKAGE_ROOT/brkchrd/licenses/THIRD_PARTY_NOTICES.md"
cp "$BUILD_DIR/brkchrd-sdl" "$PACKAGE_ROOT/brkchrd/brkchrd-sdl.aarch64"
chmod +x "$PACKAGE_ROOT/BRKCHRD.sh" "$PACKAGE_ROOT/brkchrd/brkchrd-sdl.aarch64"
(cd "$PACKAGE_ROOT" && zip -9 -r "$OUTPUT_DIR/brkchrd-aarch64-test.zip" .)
rm -rf "$STAGE"
echo "$OUTPUT_DIR/brkchrd-aarch64-test.zip"
