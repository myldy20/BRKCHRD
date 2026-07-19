# Architecture — English

## Modules

```text
music.cpp      functional chord banks, colours and voice leading
synth.cpp      polyphonic voices, ten synthesis models, play modes and FX
sdl_main.cpp   Brick input routing, dual-panel UI, persistence, diagnostics and SDL bridge
```

The core does not depend on SDL2. Unit tests and offline WAV rendering therefore work without the handheld frontend.

## Harmony path

```text
physical face position
  + persistent chord bank
  + persistent palette
  + armed or temporarily held D-pad direction
      → ChordSpec (root, intervals, name, degree, modifier)
      → inversion search / nearest voice-leading candidate
      → MIDI-note vector
```

The face-button translation is deliberately positional. Knulli's SDL mapping follows an Xbox-style semantic layout, while the Brick caps use Nintendo-style labels. BRKCHRD converts SDL positions back to the physical `X / Y / A / B` labels before presenting them to the user.

Voice leading evaluates inversions and octave shifts against the previous chord. It minimises squared pitch movement and adds a small centre-of-register penalty.

## Audio path

```text
note vector
  → 24-voice allocator
  → model-specific oscillator / excitation
  → shared state-variable body filter
  → stereo pan / spread
  → FX slot 1
  → FX slot 2
  → master gain and soft clipping
  → 48 kHz float stereo SDL callback
```

No allocation occurs in the per-sample loop. The audio callback is protected by a mutex shared with UI edits. This is simple and deterministic at the current project scale; a future version may use a lock-free command queue if physical profiling justifies it.

## Dual-panel UI state

The 512×384 logical canvas is split into two equal 238-pixel panels plus fixed header and footer regions.

```text
left panel                           right panel
CLASSIC / EXTENDED / DARK            CORE / DIATONIC+ / BORROWED
SOUND                                FX 1
SYSTEM                               FX 2
                                     MASTER
```

The last rear side pressed owns D-pad focus. Focus is a separate state from the active chord bank and palette, so editing an effect does not alter the current harmony setup.

All panel geometry uses fixed rectangles and clipped labels. Large chord labels use the original 5×7 font at scale 2–3; microtext uses scale 1 with zero extra tracking to avoid the loose appearance of the original alpha interface.

## Input routing

The mapping observed on TrimUI Brick with Knulli is handled explicitly:

- `SDL_CONTROLLER_BUTTON_LEFTSHOULDER` / `RIGHTSHOULDER`: glowing front octave controls;
- `SDL_CONTROLLER_BUTTON_LEFTSTICK` and left trigger axis: rear-left channel;
- `SDL_CONTROLLER_BUTTON_RIGHTSTICK` and right trigger axis: rear-right channel;
- SDL face-button positions: translated to physical Brick cap labels.

Rear L1/L2 therefore share the left-panel action, and rear R1/R2 share the right-panel action. Duplicate trigger/button events are edge-filtered through the stored input state.

The first controller events and raw joystick events are logged with a fixed event budget. This provides firmware diagnostics without creating an unbounded log file.

## Reactive rendering

Visual motion is derived from time, output peak and current interaction state:

- background depth grid;
- focused-panel glow;
- active chord pulse;
- output meter;
- effect-response curve;
- current chord and mode status.

`UI MOTION` selects OFF, LOW or FULL. Rendering state is read-only with respect to the audio graph.

## Persistence

`brkchrd.cfg` is a readable key/value file. It stores:

- key and octave;
- selected left and right panel modes;
- active bank, palette and armed direction;
- preset, BPM, play mode and latch;
- all seven synth parameters;
- both FX slots;
- UI motion level.

Unknown keys are ignored for forward compatibility.

## Performance targets

- logical UI: 512×384 scaled exactly to 1024×768;
- audio: 48 kHz stereo float, requested 512-frame buffer;
- maximum voices: 24;
- effect memory: two preallocated stereo circular buffers;
- no per-frame texture allocation;
- target: TrimUI Brick AArch64 / Knulli Scarab.
