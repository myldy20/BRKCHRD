# Architecture — English

## Modules

```text
music.cpp      functional chord banks, colours and voice leading
synth.cpp      polyphonic voices, ten synthesis models, sequenced play modes and FX
sdl_main.cpp   input routing, page state, persistence, drawing and SDL audio bridge
```

The core does not depend on SDL2. This allows unit tests and offline WAV rendering on systems where the handheld frontend cannot be built.

## Harmony path

```text
face button + held bank + palette + armed/live D-pad direction
    → ChordSpec (root, intervals, name, degree, modifier)
    → inversion search / nearest voice-leading candidate
    → MIDI-note vector
```

Voice leading evaluates inversions and octave shifts against the previous chord. It minimises squared pitch movement and adds a small centre-of-register penalty.

## Audio path

```text
note vector
  → 24-voice allocator
  → model-specific oscillator/excitation
  → shared state-variable body filter
  → stereo pan/spread
  → FX slot 1
  → FX slot 2
  → master gain and soft clipping
  → 48 kHz float stereo SDL callback
```

No allocation occurs in the per-sample loop. The audio callback is protected by a mutex shared with UI edits. This is simple and deterministic for the current scale of the project; a future version may replace the mutex with a lock-free command queue if hardware profiling justifies it.

## Input compatibility

The application opens SDL's GameController interface and accepts the glowing front buttons through either:

- `SDL_CONTROLLER_BUTTON_LEFTSTICK` / `RIGHTSTICK` (Knulli L3/R3 mapping);
- `SDL_CONTROLLER_AXIS_TRIGGERLEFT` / `TRIGGERRIGHT` (mapping observed in the first BRKCHRD hardware test).

Rear shoulder buttons are interpreted as momentary chord-bank modifiers when a face button is used, and as page taps otherwise.

## Persistence

`brkchrd.cfg` is a readable key/value file. It stores key, octave, preset, BPM, play mode, latch state, armed palette/direction, all seven sound controls and both FX slots.

## Performance targets

- logical UI: 512×384 scaled to 1024×768;
- audio: 48 kHz stereo float, requested 512-frame buffer;
- maximum voices: 24;
- effect memory: two preallocated stereo circular buffers;
- target: TrimUI Brick AArch64 / Knulli Scarab.
