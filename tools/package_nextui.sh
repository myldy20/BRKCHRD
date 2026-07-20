#!/bin/sh
set -eu

ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
BUILD_INPUT=${1:-"$ROOT_DIR/build-aarch64"}
OUTPUT_INPUT=${2:-"$ROOT_DIR/dist"}

BUILD_DIR=$(CDPATH= cd -- "$BUILD_INPUT" && pwd)
mkdir -p "$OUTPUT_INPUT"
OUTPUT_DIR=$(CDPATH= cd -- "$OUTPUT_INPUT" && pwd)

SOURCE="$ROOT_DIR/packaging/nextui/tg5040"
STAGE="$OUTPUT_DIR/nextui-stage"
PAK="$STAGE/Tools/tg5040/BRKCHRD.pak"
MEDIA="$STAGE/Tools/tg5040/.media"

[ -x "$BUILD_DIR/brkchrd-sdl" ] || {
  echo "missing binary: $BUILD_DIR/brkchrd-sdl" >&2
  exit 1
}

rm -rf "$STAGE"
mkdir -p "$PAK/docs" "$PAK/licenses" "$MEDIA"

cp "$SOURCE/BRKCHRD.pak/launch.sh" "$PAK/launch.sh"
cp "$SOURCE/BRKCHRD.pak/README.md" "$PAK/README.md"
cp "$BUILD_DIR/brkchrd-sdl" "$PAK/brkchrd-sdl.aarch64"
cp "$SOURCE/.media/BRKCHRD.png" "$MEDIA/BRKCHRD.png"
cp "$ROOT_DIR"/docs/*.md "$PAK/docs/"
cp "$ROOT_DIR/LICENSE" "$PAK/licenses/GPL-3.0.txt"
cp "$ROOT_DIR/NOTICE.md" "$PAK/licenses/NOTICE.md"
cp "$ROOT_DIR/THIRD_PARTY_NOTICES.md" "$PAK/licenses/THIRD_PARTY_NOTICES.md"

chmod +x "$PAK/launch.sh" "$PAK/brkchrd-sdl.aarch64"

(cd "$STAGE" && zip -9 -r "$OUTPUT_DIR/brkchrd-v0.5.2-nextui-tg5040.zip" Tools)
rm -rf "$STAGE"

echo "Created $OUTPUT_DIR/brkchrd-v0.5.2-nextui-tg5040.zip"
