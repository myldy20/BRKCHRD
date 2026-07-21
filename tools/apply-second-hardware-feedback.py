from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def replace_once(path: Path, old: str, new: str) -> None:
    text = path.read_text(encoding="utf-8")
    count = text.count(old)
    if count != 1:
        raise RuntimeError(f"expected one match in {path}, found {count}")
    path.write_text(text.replace(old, new, 1), encoding="utf-8")


sdl = ROOT / "src/sdl_main.cpp"
synth = ROOT / "src/synth.cpp"
tests = ROOT / "tests/test_main.cpp"
package = ROOT / "tools/package_portmaster.sh"
launcher = ROOT / "packaging/portmaster/brkchrd/BRKCHRD.sh"
metadata = ROOT / "packaging/portmaster/brkchrd/install_metadata.sh"

replace_once(
    sdl,
    '''void text_box(SDL_Renderer* r, int x, int y, int w, int h, const std::string& value,
              int maximum_scale, Colour c, int tracking = 1) {
    int scale = std::max(1, maximum_scale);
    while (scale > 1 && text_width(value, scale, tracking) > w - 8) --scale;
    const std::string shown = fit_width(value, w - 8, scale, tracking);
    text(r, x + w / 2, y + (h - 7 * scale) / 2, shown, scale, c, true, tracking);
}
void micro''',
    '''void text_box(SDL_Renderer* r, int x, int y, int w, int h, const std::string& value,
              int maximum_scale, Colour c, int tracking = 1) {
    int scale = std::max(1, maximum_scale);
    while (scale > 1 && text_width(value, scale, tracking) > w - 8) --scale;
    const std::string shown = fit_width(value, w - 8, scale, tracking);
    text(r, x + w / 2, y + (h - 7 * scale) / 2, shown, scale, c, true, tracking);
}
void text_box_left(SDL_Renderer* r, int x, int y, int w, int h, const std::string& value,
                   int maximum_scale, Colour c, int tracking = 1) {
    int scale = std::max(1, maximum_scale);
    while (scale > 1 && text_width(value, scale, tracking) > w - 8) --scale;
    const std::string shown = fit_width(value, w - 8, scale, tracking);
    text(r, x + 4, y + (h - 7 * scale) / 2, shown, scale, c, false, tracking);
}
void text_box_wrapped(SDL_Renderer* r, int x, int y, int w, int h, const std::string& value,
                      Colour c) {
    constexpr int scale = 2;
    constexpr int tracking = 1;
    const std::string shown = fit(value, 10);
    const int characters = brkchrd_ui::codepoint_count(shown);
    if (characters <= 5) {
        text(r, x + w / 2, y + (h - 7 * scale) / 2, shown, scale, c, true, tracking);
        return;
    }
    const int split = (characters + 1) / 2;
    const std::size_t offset = brkchrd_ui::byte_offset(shown, split);
    const std::string top = fit_width(shown.substr(0, offset), w - 6, scale, tracking);
    const std::string bottom = fit_width(shown.substr(offset), w - 6, scale, tracking);
    const int total_height = 14 * 2 + 3;
    const int first_y = y + (h - total_height) / 2;
    text(r, x + w / 2, first_y, top, scale, c, true, tracking);
    text(r, x + w / 2, first_y + 17, bottom, scale, c, true, tracking);
}
void micro''')

replace_once(
    sdl,
    '''    text_box(r, x + 3, y + 3, w - 6, h - 6, label, 2, active ? kBg : kInk, 1);''',
    '''    text_box_wrapped(r, x + 3, y + 3, w - 6, h - 6, label, active ? kBg : kInk);''')

replace_once(
    sdl,
    '''    text_box(r, x + 4, y + 27, w - 8, h - 31, label, 2, active && pulse <= 0.5F ? kBg : kInk, 1);''',
    '''    text_box_wrapped(r, x + 4, y + 23, w - 8, h - 25, label,
                     active && pulse <= 0.5F ? kBg : kInk);''')

replace_once(
    sdl,
    '''        text_box(r, 34, row_y + 2, 274, 27, settings_name(row, ui), 2, active ? kBg : kInk, 1);''',
    '''        text_box_left(r, 34, row_y + 2, 274, 27, settings_name(row, ui), 2,
                      active ? kBg : kInk, 1);''')

replace_once(
    sdl,
    '''        case Direction::DownLeft: return {fx(EffectType::Delay, 1.00F, 0.94F), fx(EffectType::Reverb, 0.74F, 0.88F)};''',
    '''        case Direction::DownLeft: return {fx(EffectType::Delay, 0.84F, 0.86F), fx(EffectType::Reverb, 0.40F, 0.72F)};''')

replace_once(
    synth,
    '''            const float feedback = 0.18F + amount * 0.62F;
            state.left[state.position] = left + dr * feedback;
            state.right[state.position] = right + dl * feedback;
            out_l = left + dl * wet * 0.62F; out_r = right + dr * wet * 0.62F;''',
    '''            const float feedback = 0.14F + amount * 0.48F;
            state.left[state.position] = std::clamp(left * 0.72F + dr * feedback, -1.25F, 1.25F);
            state.right[state.position] = std::clamp(right * 0.72F + dl * feedback, -1.25F, 1.25F);
            out_l = left * (1.0F - wet * 0.22F) + dl * wet * 0.48F;
            out_r = right * (1.0F - wet * 0.22F) + dr * wet * 0.48F;''')

replace_once(
    synth,
    '''            const float feedback = 0.54F + colour * 0.34F;
            state.left[state.position] = left + dr * feedback;
            state.right[state.position] = right + dl * feedback;
            out_l = left * (1.0F - wet * 0.30F) + (dl + dr * 0.35F) * wet * 0.50F;
            out_r = right * (1.0F - wet * 0.30F) + (dr + dl * 0.35F) * wet * 0.50F;''',
    '''            const float feedback = 0.42F + colour * 0.24F;
            state.left[state.position] = std::clamp(left * 0.68F + dr * feedback, -1.20F, 1.20F);
            state.right[state.position] = std::clamp(right * 0.68F + dl * feedback, -1.20F, 1.20F);
            out_l = left * (1.0F - wet * 0.24F) + (dl + dr * 0.28F) * wet * 0.42F;
            out_r = right * (1.0F - wet * 0.24F) + (dr + dl * 0.28F) * wet * 0.42F;''')

replace_once(
    tests,
    '''    CHECK(absolute_peak(cleared_delay) < 0.00001F);

    SynthEngine rapid(48000.0);''',
    '''    CHECK(absolute_peak(cleared_delay) < 0.00001F);

    SynthEngine deep_echo(48000.0);
    deep_echo.set_preset(1);
    deep_echo.set_effect(0, {EffectType::Delay, 0.84F, 0.86F});
    deep_echo.set_effect(1, {EffectType::Reverb, 0.40F, 0.72F});
    deep_echo.play_chord({48, 55, 60, 64});
    for (int block = 0; block < 240; ++block) {
        const auto echo_audio = deep_echo.render_copy(512);
        CHECK(std::all_of(echo_audio.begin(), echo_audio.end(), [](float sample) { return std::isfinite(sample); }));
        CHECK(absolute_peak(echo_audio) <= 1.001F);
        if (block == 24) deep_echo.release_chord();
    }

    SynthEngine rapid(48000.0);''')

metadata.write_text('''#!/bin/sh
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
    printf '%s\n' '<?xml version="1.0"?>' '<gameList>'
    cat "$ENTRY_FILE"
    printf '%s\n' '</gameList>'
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
''', encoding="utf-8")

replace_once(
    package,
    '''cp "$ASSETS/BRKCHRD.sh" "$PACKAGE/BRKCHRD.sh"
cp "$ASSETS/screenshot.jpg" "$PACKAGE/screenshot.jpg"''',
    '''cp "$ASSETS/BRKCHRD.sh" "$PACKAGE/BRKCHRD.sh"
cp "$ASSETS/install_metadata.sh" "$PACKAGE/brkchrd/install_metadata.sh"
cp "$ASSETS/screenshot.jpg" "$PACKAGE/screenshot.jpg"''')
replace_once(
    package,
    '''chmod +x "$PACKAGE/BRKCHRD.sh" "$PACKAGE/brkchrd/brkchrd-sdl.aarch64"''',
    '''chmod +x "$PACKAGE/BRKCHRD.sh" "$PACKAGE/brkchrd/brkchrd-sdl.aarch64" "$PACKAGE/brkchrd/install_metadata.sh"''')

replace_once(
    launcher,
    '''mkdir -p "$CONFDIR"
cd "$GAMEDIR" || exit 1''',
    '''mkdir -p "$CONFDIR"
if [ -x "$GAMEDIR/install_metadata.sh" ]; then
  "$GAMEDIR/install_metadata.sh" "/$directory/ports" >/dev/null 2>&1 || true
fi
cd "$GAMEDIR" || exit 1''')
