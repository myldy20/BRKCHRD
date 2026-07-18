# Research and workflow decisions

## What is worth borrowing as a concept

### HiChord

The strongest part of HiChord is not its list of features but its performance grammar:

1. chord buttons are scale degrees, not fixed note names;
2. the chord-modification joystick is temporary and operates while a chord is held;
3. key changes preserve functional relationships;
4. inversions and voice leading reduce jumps between chords;
5. block chord, strum, arpeggio and repeating modes reuse the same harmonic input.

BRKCHRD keeps that grammar but adapts it to the Brick's controls rather than simulating the physical HiChord layout literally.

### Minichord

Minichord demonstrates that an accessible chord instrument benefits from a real internal synth, editable presets, slash/alternate chord support and a physical workflow that can be understood without a piano keyboard. Its Teensy-specific audio implementation is not portable to Knulli, so BRKCHRD does not import its firmware.

### Touchord

Touchord validates scale-degree chord keys and multiple performance modes, and it shows why an open, repairable and modifiable chord instrument is useful. It is a MIDI controller, not a standalone sound engine, so its architecture is not directly suitable for the Brick.

### Gamepad chord controllers

DIMEA Arcade and several community gamepad experiments demonstrate that face buttons, sticks and rear triggers can become a credible musical interface rather than a novelty. The important lesson is separation of roles: one control family chooses harmonic function, another changes colour or tension, and rear controls select layers or modes.

## Why BRKCHRD does not simply port an existing project

The closest open projects target Teensy, RP2350, web/DAW MIDI or custom hardware. None provides all of the following together:

- native AArch64 Linux / SDL2 runtime;
- standalone audio on the TrimUI Brick;
- direct use of the built-in Knulli gamepad;
- temporary eight-direction chord transformations;
- packaging for PortMaster.

Porting an embedded firmware stack would require replacing its entire hardware, audio and UI layer. Reusing the proven SDL2/PortMaster structure from the Myldy design handheld projects is the lower-risk path.

## Control model

The Brick has fewer face buttons than HiChord's seven chord keys, but it has four rear controls. BRKCHRD treats those controls as two independent dimensions:

- L1/R1 change **which chords** are on A/B/X/Y;
- L2/R2 change **how the D-pad colours the held chord**.

This creates 12 immediately reachable base chords and 24 temporary colour operations without a modal menu.

### Core diamond

- A = I
- B = V
- X = vi
- Y = IV

This is not a numerical layout. It is a performance layout: clockwise movement plays I–V–vi–IV.

### Rear chord banks

L1 exposes the remaining diatonic material plus ♭VII. R1 exposes four common borrowed chords from the parallel minor environment. The banks are held rather than toggled, so returning to the core bank is automatic.

### D-pad colour palettes

The D-pad only changes chord colour while a face button is held. Without a held chord, the same D-pad changes global key and octave. This avoids a separate navigation screen for the two most frequent global operations.

## Sound architecture

Version 0.1 intentionally uses an original lightweight procedural synth rather than a sample library:

- up to 24 simple polyphonic voices;
- two oscillators per voice;
- per-voice envelope, stereo placement and low-pass smoothing;
- preset-level detune and timbral combinations;
- global chorus, tempo delay, compact feedback reverb and soft limiting.

The first goal is low latency and reliable execution on the Brick. Later versions can add a sample/rompler layer or a more sophisticated open DSP library after profiling the real device.

## Deliberately deferred

- looper and drum machine;
- MIDI output;
- custom chord assignment;
- editable sound parameters;
- minor-key diatonic bank as a separate global scale;
- slash-bass performance;
- user chord locks;
- sample playback.

These are useful, but adding them before validating the core instrument would bury the actual workflow under menus.
