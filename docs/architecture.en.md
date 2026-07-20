# BRKCHRD 0.4.0 Architecture — English

## Modules

```text
music.cpp      functional chord banks, colour definitions and voice leading
synth.cpp      voices, ten synthesis models, play modes and effect algorithms
sdl_main.cpp   Brick input routing, performance state, UI, persistence and SDL bridge
```

The harmony/audio core remains independent from SDL2, allowing unit tests and offline WAV rendering without the handheld frontend.

## Independent performance state

Version 0.4 separates musical state from the view currently controlled by the D-pad:

```text
stored chord colour  ───────────────┐
active base/R1/R2 bank ─────────────┼→ ChordSpec → voice leading → notes
physical ABXY position ─────────────┘

D-pad mode = CHORD | SOUND | PERF FX
```

Changing to SOUND or PERF FX therefore does not alter the stored chord colour. The colour changes only through CHORD-mode selection.

## Harmony path

```text
physical face position
  + active momentary bank (R2 > R1 > base)
  + stored colour palette/direction
      → ChordSpec
      → inversion search / nearest voice-leading candidate
      → MIDI-note vector
```

Knulli reports Xbox-style SDL face positions while the Brick uses different printed letters. BRKCHRD translates by physical position before drawing labels.

## Rear-control routing

The confirmed mapping is handled explicitly:

- front left/right shoulders: octave down/up;
- left-stick button: physical rear L1;
- left trigger: physical rear L2;
- right-stick button: physical rear R1;
- right trigger: physical rear R2.

Settings contain two logical swap flags. They exchange L1/L2 or R1/R2 roles without changing the global controller mapping.

### L1

L1 is a momentary layer flag interpreted by the active D-pad mode:

- CHORD: alternate palette display/selection;
- SOUND: alternate parameter bank;
- PERF FX: alternate performance-effect bank.

### L2

L2 is edge-triggered and cycles the D-pad state machine:

```text
CHORD → SOUND → PERF FX → CHORD
```

### R1/R2

R1 and R2 are independent momentary bank flags. Priority is R2, then R1, then the base bank. A sounding chord is rebuilt when either flag changes.

## Performance FX transaction

Momentary performance FX use a snapshot/restore transaction:

1. First non-centred D-pad direction snapshots FX1 and FX2.
2. The chosen direction and L1 layer generate a temporary effect pair.
3. Moving the D-pad replaces that pair.
4. Returning to centre restores the snapshot.
5. Leaving PERF FX, opening Settings or exiting also restores the snapshot.

This keeps destructive live gestures separate from the saved base chain.

## Audio path

```text
note vector
  → 24-voice allocator
  → model-specific oscillator/excitation
  → body filter and stereo spread
  → FX1
  → FX2
  → master soft clipping
  → 48 kHz float stereo SDL callback
```

No allocation occurs inside the per-sample loop. UI edits and the audio callback currently use the existing mutex-based synchronisation.

## UI structure

The 512×384 logical canvas retains a fixed header, two equal performance panels and footer.

- left panel follows CHORD, SOUND or PERF FX;
- right panel always shows the playable chord diamond and active bank;
- Settings is a full-screen overlay;
- labels use fixed geometry and clipping;
- animation derives from time, output peak and current interaction state.

## Persistence

`brkchrd.cfg` stores:

- key and octave;
- default and L1-held palettes;
- stored chord palette/direction;
- base, R1-held and R2-held banks;
- D-pad mode;
- preset, BPM, play mode and all synth macros;
- base FX1 and FX2;
- UI motion;
- left/right rear swap flags.

Legacy latch is read only for compatibility and forced off.

## Performance targets

- logical UI: 512×384 scaled to 1024×768;
- audio: 48 kHz stereo float, requested 512-frame buffer;
- maximum voices: 24;
- two preallocated effect buffers;
- bounded controller-event logging;
- target: TrimUI Brick AArch64 / Knulli Scarab.
