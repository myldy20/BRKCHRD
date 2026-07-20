#!/bin/sh
set -eu

ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
BUILD_DIR=${1:-"$ROOT_DIR/build-aarch64"}
OUTPUT_DIR=${2:-"$ROOT_DIR/dist"}
SOURCE="$ROOT_DIR/packaging/nextui/tg5040"
STAGE="$OUTPUT_DIR/nextui-stage"
PAK="$STAGE/Tools/tg5040/BRKCHRD.pak"
MEDIA="$STAGE/Tools/tg5040/.media"

rm -rf "$STAGE"
mkdir -p "$PAK/docs" "$PAK/licenses" "$MEDIA" "$OUTPUT_DIR"

cp "$SOURCE/BRKCHRD.pak/launch.sh" "$PAK/launch.sh"
cp "$SOURCE/BRKCHRD.pak/README.md" "$PAK/README.md"
cp "$BUILD_DIR/brkchrd-sdl" "$PAK/brkchrd-sdl.aarch64"
cp "$SOURCE/.media/BRKCHRD.png" "$MEDIA/BRKCHRD.png"
cp "$ROOT_DIR"/docs/*.md "$PAK/docs/"
cp "$ROOT_DIR/LICENSE" "$PAK/licenses/GPL-3.0.txt"
cp "$ROOT_DIR/NOTICE.md" "$PAK/licenses/NOTICE.md"
cp "$ROOT_DIR/THIRD_PARTY_NOTICES.md" "$PAK/licenses/THIRD_PARTY_NOTICES.md"

chmod +x "$PAK/launch.sh" "$PAK/brkchrd-sdl.aarch64"

(cd "$STAGE" && zip -9 -r "$OUTPUT_DIR/brkchrd-v0.5.1-nextui-tg5040.zip" Tools)
rm -rf "$STAGE"

echo "Created $OUTPUT_DIR/brkchrd-v0.5.1-nextui-tg5040.zip"
