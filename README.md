# BRKCHRD

**BRKCHRD** is an open-source chord-performance instrument for TrimUI Brick. Four face buttons play functional chords while the D-pad selects chord colours, edits the synth or performs momentary effects.

Current version: **0.5.0 smart voicing preview**.

**English:** [Manual](docs/manual.en.md) · [Controls](docs/controls.en.md) · [Sound design](docs/sound-design.en.md) · [Installation](docs/install.en.md) · [Architecture](docs/architecture.en.md) · [Troubleshooting](docs/troubleshooting.en.md) · [Hardware test](docs/hardware-test-checklist.en.md)

**Русский:** [Руководство](docs/manual.ru.md) · [Управление](docs/controls.ru.md) · [Звуковой движок](docs/sound-design.ru.md) · [Установка](docs/install.ru.md) · [Архитектура](docs/architecture.ru.md) · [Диагностика](docs/troubleshooting.ru.md) · [Аппаратный тест](docs/hardware-test-checklist.ru.md)

## What changed in 0.5

Version 0.5 addresses two musical problems found on the physical Brick:

1. the old voice-leading algorithm let the order of ABXY presses move the same chord into different octaves and could partially cancel an explicit octave change;
2. every synth received the same dense voicing, so low or distorted instruments could turn extended chords into mud and dissonant output.

### Stable or live voicing

`VOICE LEAD` is now a separate setting.

- **OFF — default:** one button, bank, colour and octave always produce the same notes. Changing octave moves the complete voicing by exactly 12 semitones.
- **ON:** BRKCHRD searches nearby inversions based on the previous chord, preserving the expressive clockwise/counter-clockwise behaviour. The search is now anchored to the selected octave and cannot freely drag the chord back into the old register.

### Instrument-aware chord spacing

Each factory preset declares a voicing profile:

- Keys and Organ use clear four-note shell voicings;
- Pad and Choir can use wider five-note voicings;
- Pluck raises the useful register and removes unnecessary inner notes;
- Heavy keeps power/shell information instead of stacking dense low extensions;
- Bass reduces chords to root, fifth and octave.

Chord construction is also quality-aware. Minor and diminished scale degrees now receive appropriate minor, half-diminished or diminished extensions rather than forced major structures.

## Sixteen factory sounds

The factory set was rebuilt for chord playing:

1. Amber Keys
2. Analog Bloom
3. Moon Organ
4. Cinema Strings
5. Velvet Choir
6. Muted Pluck
7. Glass Harp
8. Noir Reed
9. Dust Piano
10. String Haze
11. Chapel Air
12. Tape Choir
13. Doom Chords
14. Sub Altar
15. Frozen Glass
16. Wire Pluck

The new patches use less aggressive FM/noise/detune, individual output compensation and chord-count normalisation. Doom Chords and Sub Altar deliberately simplify dense extensions to keep the low end usable.

## Performance controls

| Control | Action |
| --- | --- |
| glowing front left/right | octave down/up |
| **L1 press** | cycle `CHORD → SOUND → PERF FX` |
| **L2 hold** | alternate palette, sound bank or PERF FX bank |
| R1/R2 hold | configured alternate chord banks |
| D-pad | chord colour, sound editing or momentary performance FX |
| ABXY | play functional chords |
| Select | open/close Settings |
| Start tap | PAD / STRUM / ARP / PULSE |
| Start hold | all notes off |
| Start + Select | save and exit |

The old L1/L2 duties have been exchanged. `SWAP L1/L2` remains available for firmware/controller-order differences.

## D-pad modes

### CHORD

D-pad stores a chord colour. L2 held exposes the configured alternate palette. The selected colour remains active while SOUND, PERF FX or Settings is open.

### SOUND

Up/Down selects a parameter and Left/Right edits it.

Normal layer: Preset, Tone, Body and Motion.

L2-held layer: Attack, Release, Spread, BPM and Play Mode.

### PERF FX

Holding a D-pad direction temporarily substitutes a two-effect performance gesture. Releasing the direction restores the saved base FX. L2 held selects the second performance bank.

`PERF FX` can be disabled in Settings. When disabled, L1 cycles only `CHORD ↔ SOUND`, and a previously saved PERF FX screen is replaced by CHORD on startup.

## Settings

The Select menu includes:

- base and L2-held colour palettes;
- base, R1 and R2 chord banks;
- both base FX slots;
- key and octave;
- `VOICE LEAD` On/Off;
- `PERF FX` On/Off;
- UI motion;
- rear-button swap options.

Only one output meter remains: the compact meter in the top-right corner.

## Startup

Every normal launch shows a two-second splash:

```text
BRKCHRD
developed by myldy design
@myldy20
```

Automated headless tests skip the delay through `BRKCHRD_SKIP_SPLASH=1`.

## Install on Knulli / PortMaster

Download `brkchrd-v050-portmaster-aarch64` from a successful workflow run and extract the inner archive into `/userdata/roms/ports/`.

```text
/userdata/roms/ports/BRKCHRD.sh
/userdata/roms/ports/brkchrd/brkchrd-sdl.aarch64
```

Runtime files:

```text
/userdata/roms/ports/brkchrd/conf/brkchrd.cfg
/userdata/roms/ports/brkchrd/conf/brkchrd.log
```

## Build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j2
ctest --test-dir build --output-on-failure
./build/brkchrd-sdl
```

BRKCHRD is GPL-3.0-or-later software developed by **Myldy design** — [@myldy20](https://github.com/myldy20).
