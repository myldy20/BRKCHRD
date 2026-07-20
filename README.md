# BRKCHRD

**BRKCHRD** is an open-source chord-performance instrument for TrimUI Brick. It turns the handheld into a self-contained harmony synth with functional chord banks, armable chord colours, automatic voice leading, ten synthesis engines, editable base effects and momentary performance FX.

Current version: **0.4.0 performance workflow preview**.

**English:** [Manual](docs/manual.en.md) · [Controls](docs/controls.en.md) · [Sound design](docs/sound-design.en.md) · [Installation](docs/install.en.md) · [Architecture](docs/architecture.en.md) · [Troubleshooting](docs/troubleshooting.en.md) · [Hardware test](docs/hardware-test-checklist.en.md)

**Русский:** [Руководство](docs/manual.ru.md) · [Управление](docs/controls.ru.md) · [Звуковой движок](docs/sound-design.ru.md) · [Установка](docs/install.ru.md) · [Архитектура](docs/architecture.ru.md) · [Диагностика](docs/troubleshooting.ru.md) · [Аппаратный тест](docs/hardware-test-checklist.ru.md)

## Why 0.4 exists

Version 0.3 proved the visual system, but treated every rear control as a page switch. Physical testing and the controller log showed that the Brick exposes four distinct rear controls: L1/R1 through stick-button mappings and L2/R2 through trigger-button mappings. Version 0.4 uses them as performance controls rather than navigation.

## Performance model

The right half always remains a playable ABXY chord diamond. The left half changes according to the current D-pad mode.

### CHORD

- D-pad selects and stores a chord colour.
- L1 held shows an alternate colour palette.
- Releasing L1 returns the display to the default palette, but does not erase the selected chord colour.
- The stored colour continues to affect every chord while SOUND or PERF FX is open.

The default palette and the L1-held palette are independently configurable as CLASSIC, EXTENDED or DARK.

### SOUND

L2 cycles into SOUND. D-pad Up/Down selects a parameter and Left/Right edits it.

- normal layer: Preset, Tone, Body and Motion;
- L1 held: Attack, Release, Spread, BPM and Play Mode.

ABXY stay playable during editing, and the previously selected maj7, power, sus, add9 or other chord colour remains active.

### PERF FX

L2 cycles again into PERF FX. Holding a D-pad direction temporarily replaces the two normal FX slots with a performance combination. Releasing the direction restores the saved base FX exactly.

- normal layer: cuts, stutters, chopping, crushing, drive, wash, deep echo and phase combinations;
- L1 held: a second, more extreme bank with abyssal delay/reverb, ratetrap, glitch, broken, doom, shimmer-like, tunnel and alien combinations.

This is designed as a sampler-style live gesture system rather than a menu of static effect presets.

## Chord banks

R1 and R2 are momentary chord-bank modifiers. The default, R1-held and R2-held banks can each be configured as CORE, DIATONIC+ or BORROWED.

The physical button layout is:

```text
        X
Y               A
        B
```

The default CORE mapping remains:

```text
        X = IV
Y = vi          A = V
        B = I
```

## Controls

| Control | Action |
| --- | --- |
| glowing front left | octave down |
| glowing front right | octave up |
| L1 hold | alternate layer for CHORD, SOUND or PERF FX |
| L2 press | cycle CHORD → SOUND → PERF FX |
| R1 hold | configured alternate chord bank 1 |
| R2 hold | configured alternate chord bank 2 |
| D-pad | chord colour, sound edit or momentary performance FX |
| ABXY | play functional chords in the active bank |
| Select | open/close Settings |
| Start tap | next PAD / STRUM / ARP / PULSE mode |
| Start hold | all notes off |
| Start + Select | save and exit |

Latch has been removed. A chord releases when its last ABXY button is released.

## Settings

Settings opens with Select and includes:

- default and L1-held colour palettes;
- default, R1-held and R2-held chord banks;
- both base FX slots: Type, Amount and Colour;
- key and octave;
- UI motion: OFF, LOW or FULL;
- SWAP L1/L2 and SWAP R1/R2 for firmware/controller-order differences.

## Sound engine

Ten synthesis engines remain available: Velvet Poly, Dust Piano, Chapel Organ, String Haze, Tape Choir, Wire Pluck, Frozen Glass, Doom Stack, Smoke Reed and Sub Altar.

Editable synthesis controls: Tone, Body, Motion, Attack, Release and Spread. Two serial base effect slots support Off, Chorus, Phaser, Tremolo, Drive, Crusher, Delay and Reverb.

## Install on Knulli / PortMaster

Download the latest successful `brkchrd-v040-portmaster-aarch64` workflow artifact and extract the inner `brkchrd-v0.4.0-portmaster.zip` into:

```text
/userdata/roms/ports/
```

Expected files:

```text
/userdata/roms/ports/BRKCHRD.sh
/userdata/roms/ports/brkchrd/brkchrd-sdl.aarch64
```

Runtime files:

```text
/userdata/roms/ports/brkchrd/conf/brkchrd.cfg
/userdata/roms/ports/brkchrd/conf/brkchrd.log
```

The log records the SDL mapping, raw controller events and the expected v0.4 control roles.

## Build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j2
ctest --test-dir build --output-on-failure
./build/brkchrd-sdl
```

BRKCHRD is GPL-3.0-or-later software developed by **Myldy design** — [@myldy20](https://github.com/myldy20).
