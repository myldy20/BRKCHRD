# BRKCHRD 0.5.0 Manual — English

## Overview

BRKCHRD is a live harmony instrument for TrimUI Brick. Physical ABXY play functional chords; D-pad chooses a chord colour, edits the synth or performs momentary effects; R1/R2 temporarily change scale-degree banks.

Version 0.5 adds corrected chord extensions, sixteen chord-focused presets, instrument-aware note spacing and a separate Voice Lead switch.

## First minute

1. Launch the app and wait for the two-second Myldy design splash.
2. Play B, A, Y and X: in CORE they are I, V, vi and IV.
3. Select a colour with D-pad in CHORD.
4. Press L1 to open SOUND. ABXY retain the selected colour.
5. Use Up/Down and Left/Right to edit Preset, Tone, Body or Motion.
6. Hold L2 for Attack, Release, Spread, BPM and Play Mode.
7. Press L1 again for PERF FX, unless disabled in Settings.
8. Press Select and choose deterministic or live voicing.

## Stable versus live chords

### Voice Lead Off

This is the default and the predictable instrument mode. One button with the same bank, colour, octave and preset always produces the same notes. The order in which you visit ABXY has no effect. Octave down/up shifts every voice exactly 12 semitones.

### Voice Lead On

This restores expressive route-sensitive inversion selection. Clockwise and counter-clockwise progressions may place the same chord differently. Unlike 0.4, the search is constrained near the octave selected by the front controls.

## Preset-aware voicing

A chord symbol is not sent unchanged to every synth. The preset profile orchestrates it:

- Keys/Organ: compact clear shells;
- Pad/Choir: wider voicings up to five notes;
- Pluck: fewer notes in a higher useful register;
- Heavy: power/shell voicings without muddy low extensions;
- Bass: root, fifth and octave.

This prevents a chord that works on strings from overloading a sub or distorted stack.

## Chord colours

CLASSIC, EXTENDED and DARK now preserve the quality of the current scale degree. Minor degrees receive minor extensions; diminished degrees receive diminished or half-diminished structures. The displayed name reflects the harmonic chord while the preset profile may omit redundant notes for clarity.

## Controls

- front glowing left/right: octave down/up;
- L1 press: CHORD → SOUND → PERF FX;
- L2 hold: alternate palette/parameter/effect layer;
- R1/R2 hold: alternate chord banks;
- Select: Settings;
- Start tap: PAD / STRUM / ARP / PULSE;
- Start hold: all notes off;
- Start + Select: save and exit.

## PERF FX

PERF FX temporarily substitutes two performance effects while a D-pad direction is held. Releasing the direction restores normal FX1/FX2. Set `PERF FX = OFF` to remove the screen from the L1 cycle; ordinary effects remain available.

## Settings

Settings contains base/L2 palettes, base/R1/R2 banks, both base effects, key, octave, Voice Lead, PERF FX, UI Motion and rear-button swap options.

## Output display

Only the compact top-right output meter remains. The former second meter below the chord buttons was redundant and has been removed.

## Saving

Closing Settings saves the configuration. Start+Select saves and exits. Old 0.4 values are read where compatible; Voice Lead defaults to Off and PERF FX defaults to On when absent.
