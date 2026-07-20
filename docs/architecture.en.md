# BRKCHRD 0.5.1 Architecture — English

## Modules

```text
music.cpp      chord banks, colour definitions, deterministic voicing and live voice leading
synth.cpp      voice allocator, synthesis models, play modes and effect algorithms
sdl_main.cpp   Brick input routing, UI, persistence, SDL audio/video bridge
```

The harmony/audio core does not depend on SDL2, so unit tests and offline WAV rendering can run without the handheld frontend.

## State flow

Musical state is independent from the current D-pad screen:

```text
physical face button
  + active bank (R2 > R1 > base)
  + stored palette and direction
  + key and octave
      → ChordSpec
      → preset voicing profile
      → deterministic voicing or anchored live voice leading
      → MIDI-note vector
```

Changing from CHORD to SOUND or PERF FX does not erase the stored chord colour.

## Chord construction

`music.cpp` resolves the physical face position through the selected bank, preserving the degree quality when applying CLASSIC, EXTENDED or DARK colour. Interval lists are normalised before voicing.

`voice_chord()` has two paths:

- deterministic: ignores previous notes and applies the preset's Keys, Organ, Pad, Choir, Pluck, Heavy or Bass profile;
- live: evaluates inversions near the previous chord, while constraining candidates around the octave-selected target register.

## Input routing

The tested Knulli mapping exposes four distinct rear controls:

- front shoulders: octave down/up;
- physical L1: SDL left-stick button;
- physical L2: SDL left trigger;
- physical R1: SDL right-stick button;
- physical R2: SDL right trigger.

Current roles:

- L1 is edge-triggered and cycles CHORD → SOUND → PERF FX;
- L2 is a held alternate-layer flag interpreted by the current screen;
- R1/R2 are held chord-bank flags, with R2 priority;
- Settings can swap left or right rear roles for other firmware mappings.

## Performance FX transaction

Momentary performance FX use snapshot/restore semantics:

1. the first non-centred direction snapshots FX1 and FX2;
2. direction and L2 layer choose a temporary pair;
3. moving the D-pad replaces the temporary pair;
4. returning to centre restores the snapshot;
5. leaving PERF FX, disabling it, opening Settings or exiting also restores the snapshot.

The saved base chain is therefore not overwritten by a live gesture.

## Audio path

```text
note vector
  → bounded voice allocator
  → model-specific oscillator/excitation
  → envelope, body filter and stereo spread
  → chord-count normalisation and preset level trim
  → FX1
  → FX2
  → master soft clipping
  → 48 kHz float stereo SDL callback
```

Effect delay buffers are preallocated. Per-sample processing performs no file I/O.

## UI structure

The logical canvas is 512×384 and scales to the device display.

- fixed header with key, mode, octave and one output meter;
- left panel follows CHORD, SOUND or PERF FX;
- right panel always shows the playable face-button diamond;
- Settings is a full-screen overlay;
- footer displays physical-button actions;
- text uses an embedded bitmap font.

In 0.5.1 the preset uses a dedicated 52-pixel card, right-side values use `text_right`, and active parameter bars use a contrasting fill.

## Persistence

The configuration stores:

- key and octave;
- normal and L2 palettes;
- selected palette/direction;
- base, R1 and R2 banks;
- current D-pad mode;
- preset, BPM, play mode and synthesis macros;
- FX1 and FX2;
- Voice Lead and PERF FX switches;
- UI motion;
- rear-button swap flags.

PortMaster and NextUI launchers set different persistent paths but use the same configuration format.

## Packaging

The same stripped AArch64 executable is packaged twice:

- PortMaster/Knulli: launcher and assets under the ports tree;
- NextUI: `Tools/tg5040/BRKCHRD.pak` plus sibling `.media/BRKCHRD.png`.

Both archives include English/Russian documentation, GPLv3, `NOTICE.md` and third-party notices.

## Performance targets

- logical UI: 512×384;
- audio: 48 kHz float stereo, 512-frame requested buffer;
- maximum voices: 24;
- two preallocated effect buffers;
- bounded input logging;
- primary target: TrimUI Brick AArch64.
