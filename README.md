# BRKCHRD

**BRKCHRD** is an open-source chord-performance instrument for TrimUI Brick. It turns the handheld into a self-contained harmony synth with functional chord banks, armable chord colours, automatic voice leading, ten synthesis engines and two editable effect slots.

Current version: **0.3.0 Brick-native interface preview**.

**English:** [Manual](docs/manual.en.md) · [Controls](docs/controls.en.md) · [Sound design](docs/sound-design.en.md) · [Installation](docs/install.en.md) · [Architecture](docs/architecture.en.md) · [Troubleshooting](docs/troubleshooting.en.md) · [Hardware test](docs/hardware-test-checklist.en.md)

**Русский:** [Руководство](docs/manual.ru.md) · [Управление](docs/controls.ru.md) · [Звуковой движок](docs/sound-design.ru.md) · [Установка](docs/install.ru.md) · [Архитектура](docs/architecture.ru.md) · [Диагностика](docs/troubleshooting.ru.md) · [Аппаратный тест](docs/hardware-test-checklist.ru.md)

## Why 0.3 exists

Physical testing showed that Knulli does not expose the Brick as a conventional gamepad:

- the glowing front controls arrive as shoulder buttons;
- rear L1/L2 share one left-side channel;
- rear R1/R2 share one right-side channel;
- SDL face-button letters are swapped relative to the caps printed on the Brick.

Version 0.3 embraces that mapping instead of fighting it.

## Brick-native workflow

The screen is permanently divided into two equal, independent panels.

### Left panel — rear L1 or L2

Each press cycles:

`CLASSIC → EXTENDED → DARK → SOUND → SYSTEM`

The first three modes select the chord-colour palette. SOUND edits the synthesis engine and macros. SYSTEM edits key and interface motion.

### Right panel — rear R1 or R2

Each press cycles:

`CORE → DIATONIC+ → BORROWED → FX 1 → FX 2 → MASTER`

The first three modes select the active chord bank. FX 1 and FX 2 edit the serial effects. MASTER edits the final level and shows the signal path.

The last rear side pressed becomes focused and receives the D-pad.

## Physical controls

| Control | Action |
| --- | --- |
| glowing front left | octave down |
| glowing front right | octave up |
| rear L1 or L2 | next left-panel mode |
| rear R1 or R2 | next right-panel mode |
| D-pad | arm/hold chord colour or edit the focused panel |
| physical B, bottom | first functional chord |
| physical A, right | second functional chord |
| physical Y, left | third functional chord |
| physical X, top | fourth functional chord |
| Start | next PAD / STRUM / ARP / PULSE mode |
| short Select | latch on/off |
| hold Select | all notes off |
| Start + Select | save and exit |

The core bank is arranged by physical position:

```text
        X = IV
Y = vi          A = V
        B = I
```

ABXY remain playable while SOUND, SYSTEM, FX or MASTER is being edited.

## Interface

The 0.3 interface keeps the 512×384 logical canvas that scales exactly to the Brick's 1024×768 display, but replaces the uneven page layout with:

- two equal 238-pixel panels;
- equal-sized chord and colour cells;
- compact tracked microtext instead of oversized small labels;
- a focused-panel double outline;
- animated background depth;
- pulsing active chord controls;
- reactive output meter;
- animated FX response curves;
- live current-chord and mode feedback.

UI motion can be set to OFF, LOW or FULL under SYSTEM.

## Sound engine

Ten distinct synthesis engines:

1. Velvet Poly
2. Dust Piano
3. Chapel Organ
4. String Haze
5. Tape Choir
6. Wire Pluck
7. Frozen Glass
8. Doom Stack
9. Smoke Reed
10. Sub Altar

Editable synthesis macros: Tone, Body, Motion, Attack, Release and Spread.

Two serial effect slots support Off, Chorus, Phaser, Tremolo, Drive, Crusher, Delay and Reverb. MASTER controls the final output level.

## Install on Knulli / PortMaster

Download the latest successful `brkchrd-v030-portmaster-aarch64` workflow artifact and extract the inner installation ZIP into:

```text
/userdata/roms/ports/
```

Expected files:

```text
/userdata/roms/ports/BRKCHRD.sh
/userdata/roms/ports/brkchrd/brkchrd-sdl.aarch64
```

Refresh the game list or reboot, then open **Ports → BRKCHRD**.

Runtime configuration and logs:

```text
/userdata/roms/ports/brkchrd/conf/brkchrd.cfg
/userdata/roms/ports/brkchrd/conf/brkchrd.log
```

The log includes the detected SDL controller mapping and a limited raw input trace for hardware diagnostics.

## Build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j2
ctest --test-dir build --output-on-failure
./build/brkchrd-sdl
```

Render the ten-engine demonstration:

```bash
./build/brkchrd-render brkchrd-demo.wav
```

## Status

Automated validation covers Linux SDL2, tests, dummy video/audio startup, WAV rendering and an Ubuntu 20.04-compatible AArch64 PortMaster package. Physical validation is still required for the final visual balance, controller feel, speaker voicing, latency and CPU load.

BRKCHRD is GPL-3.0-or-later software developed by **Myldy design** — [@myldy20](https://github.com/myldy20).
