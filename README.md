# BRKCHRD

**BRKCHRD** is an open-source chord performance instrument for the TrimUI Brick. It turns the console into a self-contained harmony synth: four chord buttons, temporary or armed D-pad colours, alternate chord banks, ten distinct synthesis engines, two editable effect slots and a compact live interface designed for a 1024×768 handheld screen.

Current version: **0.2.0 product preview**.

**English:** [Manual](docs/manual.en.md) · [Controls](docs/controls.en.md) · [Sound design](docs/sound-design.en.md) · [Installation](docs/install.en.md) · [Architecture](docs/architecture.en.md) · [Troubleshooting](docs/troubleshooting.en.md)

**Русский:** [Руководство](docs/manual.ru.md) · [Управление](docs/controls.ru.md) · [Звуковой движок](docs/sound-design.ru.md) · [Установка](docs/install.ru.md) · [Архитектура](docs/architecture.ru.md) · [Диагностика](docs/troubleshooting.ru.md)

## What changed after the alpha

- the glowing front buttons are now shown as **FX1/FX2**, not L2/R2;
- FX input accepts both Knulli mappings observed on the Brick: L3/R3 buttons and trigger axes;
- the PLAY-page D-pad never changes key or octave;
- a colour can be armed before the first chord, including POWER, CRUNCH, OPEN and CLUSTER;
- key and octave moved to the SYSTEM page;
- the interface follows the visual system of Cursed Drone: cream text, purple/functional accents, dense fixed grids and full-row focus;
- one overloaded screen became four pages: PLAY, SOUND, FX and SYSTEM;
- ten synthesis engines replace the original family of nearly identical two-oscillator patches;
- SOUND exposes tone, body, motion, attack, release, spread, BPM and play mode;
- FX exposes two serial slots, eight effect types and a master level;
- documentation is maintained in English and Russian.

## Quick controls

| Action | TrimUI Brick |
| --- | --- |
| play core chords | A / B / X / Y |
| alternate chord bank | hold rear L or R while playing a chord |
| arm chord colour | tap D-pad direction on PLAY |
| return to BASE | tap the armed direction again |
| select EXTENDED palette | glowing FX1 |
| select DARK palette | glowing FX2 |
| return to CLASSIC / BASE | press FX1 + FX2 |
| previous / next page | tap rear L / R without a chord |
| select/edit parameter | D-pad Up/Down and Left/Right |
| next play mode | Start |
| latch | Select |
| save and exit | Start + Select |

The core face-button progression is deliberately clockwise:

```text
A → B → X → Y
I → V → vi → IV
```

## Install on Knulli / PortMaster

Download the latest successful `brkchrd-portmaster-aarch64` workflow artifact. GitHub wraps the actual installation archive in an outer ZIP. Extract the inner `brkchrd-v0.2.0-portmaster.zip` into:

```text
/userdata/roms/ports/
```

Expected files:

```text
/userdata/roms/ports/BRKCHRD.sh
/userdata/roms/ports/brkchrd/brkchrd-sdl.aarch64
```

Then refresh the game list or reboot and open **Ports → BRKCHRD**.

Runtime configuration and logs:

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

Render the ten-engine demonstration:

```bash
./build/brkchrd-render brkchrd-v020-demo.wav
```

## Status

Automated builds cover Linux SDL2, tests, dummy video/audio startup, WAV rendering and an Ubuntu 20.04-compatible AArch64 PortMaster package. Physical validation is still required for final controller ergonomics, speaker voicing, audio latency and CPU load.

BRKCHRD is GPL-3.0-or-later software developed by **Myldy design** — [@myldy20](https://github.com/myldy20).
