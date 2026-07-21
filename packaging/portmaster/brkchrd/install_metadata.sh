#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu

PORTS_ROOT=${1:-/userdata/roms/ports}
GAMELIST="$PORTS_ROOT/gamelist.xml"
ENTRY_FILE="$PORTS_ROOT/brkchrd/.brkchrd-game-entry.xml"
TEMP_FILE="$GAMELIST.brkchrd.tmp"

mkdir -p "$PORTS_ROOT/brkchrd"
cat > "$ENTRY_FILE" <<'EOF'
  <game>
    <path>./BRKCHRD.sh</path>
    <name>BRKCHRD</name>
    <desc>Open-source chord performance instrument for handheld consoles.</desc>
    <image>./brkchrd/cover.jpg</image>
    <releasedate>20260720T000000</releasedate>
    <developer>Myldy design</developer>
    <publisher>Myldy design</publisher>
    <genre>Music</genre>
  </game>
EOF

if [ ! -f "$GAMELIST" ]; then
  {
    printf '%s
' '<?xml version="1.0"?>' '<gameList>'
    cat "$ENTRY_FILE"
    printf '%s
' '</gameList>'
  } > "$TEMP_FILE"
else
  awk -v entry="$ENTRY_FILE" '
    function emit_entry( line) {
      while ((getline line < entry) > 0) print line
      close(entry)
    }
    /<game>/ { in_game=1; block=$0 ORS; next }
    in_game {
      block=block $0 ORS
      if ($0 ~ /<\/game>/) {
        if (block !~ /<path>[.]\/BRKCHRD[.]sh<\/path>/) printf "%s", block
        in_game=0; block=""
      }
      next
    }
    /<\/gameList>/ { emit_entry(); print; inserted=1; next }
    { print }
    END {
      if (!inserted) {
        emit_entry()
        print "</gameList>"
      }
    }
  ' "$GAMELIST" > "$TEMP_FILE"
fi

mv "$TEMP_FILE" "$GAMELIST"
rm -f "$ENTRY_FILE"
