# BRKCHRD

**BRKCHRD** is an open-source chord performance instrument for the TrimUI Brick. It turns the console's face buttons, D-pad and four rear controls into a self-contained harmony synth inspired by the immediate workflow of HiChord, Omnichord-style instruments and modern gamepad chord controllers.

Current version: **0.1.0 alpha**.

## What already works

- native SDL2 application for Linux handhelds and desktop testing;
- four core chords on `A/B/X/Y`: `I / V / vi / IV`;
- two rear-button chord banks with the remaining diatonic and borrowed chords;
- three D-pad colour palettes with 24 temporary chord transformations;
- automatic voice leading and inversion selection;
- eight procedural synth presets, with no sample files required;
- pad, strum, arpeggio and pulse play modes;
- latch/drone mode;
- key and octave changes without opening a menu;
- 48 kHz stereo synthesis with chorus, tempo delay and a compact reverb;
- settings autosave;
- PortMaster/Knulli AArch64 package built by GitHub Actions.

## TrimUI Brick controls

### Chords

| Control | Core bank | Hold L1 | Hold R1 |
| --- | --- | --- | --- |
| A | I | ii | i (borrowed) |
| B | V | iii | iv (borrowed) |
| X | vi | vii° | ♭VI |
| Y | IV | ♭VII | ♭III |

The core diamond is intentionally `A → B → X → Y = I → V → vi → IV`, so the most common four-chord progression can be played clockwise around the face buttons.

### Chord colour

Hold a face button and move the D-pad. The chord changes only while the direction is held, then returns to its base form.

- no trigger: classic colours — flip major/minor, dominant 7, maj/min 7, add9, sus4, 6/sus2, dark/dim, augmented;
- hold L2: extended colours — add11, dom9, 6/9, min11, 7sus4, half-diminished, minor-major 7, maj7#11;
- hold R2: dark colours — power chord, 7#9, quartal, add♭9 cluster, open no-third, minor 6, tritone, augmented-major 7.

### Global performance controls

| Action | Control |
| --- | --- |
| change key | D-pad Left / Right while no chord is held |
| change octave | D-pad Up / Down while no chord is held |
| next play mode | Start |
| latch / drone | Select |
| next sound | L1 + R1 while no chord is held |
| previous sound | L2 + R2 while no chord is held |
| save and exit | hold Start + Select |

Knulli may expose L2/R2 as trigger axes rather than buttons; BRKCHRD supports that SDL mapping directly.

## Install on Knulli / PortMaster

1. Open the latest successful GitHub Actions run.
2. Download the artifact `brkchrd-portmaster-aarch64`.
3. Extract the inner archive `brkchrd-aarch64-test.zip` into:

```text
/userdata/roms/ports/
```

The resulting layout should include:

```text
/userdata/roms/ports/BRKCHRD.sh
/userdata/roms/ports/brkchrd/brkchrd-sdl.aarch64
```

Refresh the game list or reboot, then open **Ports → BRKCHRD**.

Logs and settings are stored in:

```text
/userdata/roms/ports/brkchrd/conf/
```

## Desktop controls

For testing without a handheld:

- `Z/X/A/S` = console `A/B/X/Y`;
- arrows = D-pad;
- `Q/W` = L1/R1;
- `E/R` = L2/R2;
- Enter = Start;
- Space = Select;
- `[` / `]` = previous / next sound;
- Esc = exit.

## Build

The harmony and synth core has no external dependency. SDL2 is only required for the interactive frontend.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j2
ctest --test-dir build --output-on-failure
./build/brkchrd-sdl
```

Render a short WAV demonstration without SDL:

```bash
./build/brkchrd-render brkchrd-demo.wav
```

## Design notes

The research and workflow rationale are documented in [`docs/research-and-workflow.md`](docs/research-and-workflow.md).

BRKCHRD does not copy HiChord firmware or proprietary assets. It implements a distinct interface for the TrimUI Brick using public musical concepts, an original harmony engine and an original synth engine.

## Licence

BRKCHRD is licensed under GNU GPL v3.0 or later.

Developed by **Myldy design** — [@myldy20](https://github.com/myldy20).
